#include "PythonMachine.h"
#include "PythonProcess.h"

#include <cstdio>
#include <stdexcept>

#ifdef __linux__
#include <cerrno>
#include <chrono>
#include <filesystem>
#include <functional>
#include <mutex>
#include <optional>
#include <system_error>
#include <thread>
#include <type_traits>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <spawn.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

extern char** environ;
#endif

using namespace ofsim_python_integration;

namespace
{
PythonMessage handleRequest(PythonMessage message, u64 runId)
{
    auto& events = *ofsim_events::EventProcessor::getInstance();
    try
    {
        switch (message.operation)
        {
        case PythonOperation::TELEMETRY:
            message.telemetry = events.getTelemetry();
            message.shouldStop = events.isPythonMachineTerminated();
            break;
        case PythonOperation::THRUST:
            events.setThrustMagnitude(runId, message.value.x);
            break;
        case PythonOperation::ROTATION:
            events.changeThrustRotatation(runId, message.value);
            break;
        default:
            throw std::runtime_error("Invalid Python worker request");
        }
    }
    catch (const std::exception& error)
    {
        message.operation = PythonOperation::ERROR;
        std::snprintf(message.error, sizeof(message.error), "%s", error.what());
    }
    return message;
}

#ifdef __linux__
using Clock = std::chrono::steady_clock;
using namespace std::chrono_literals;
constexpr auto stopTimeout = 1s;
constexpr auto startupTimeout = 5s;
constexpr u32 maxSourceSize = 4 * 1024 * 1024;

struct Stopped
{
};

struct Channel
{
    explicit Channel(int descriptor) : descriptor(descriptor) {}
    ~Channel()
    {
        close(descriptor);
    }
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;

    void transfer(void* data, std::size_t size, bool writing) const
    {
        auto* bytes = static_cast<char*>(data);
        while (size > 0)
        {
            tick();
            pollfd state{descriptor, static_cast<short>(writing ? POLLOUT : POLLIN), 0};
            const int ready = poll(&state, 1, 20);
            if (ready < 0 && errno == EINTR)
                continue;
            if (ready < 0)
                throw std::system_error(errno, std::generic_category(), "Python worker poll");
            if (ready == 0)
                continue;

            const auto count = writing ? send(descriptor, bytes, size, MSG_NOSIGNAL)
                                       : recv(descriptor, bytes, size, 0);
            if (count < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK))
                continue;
            if (count <= 0)
                throw std::runtime_error("Python worker disconnected");
            bytes += count;
            size -= count;
        }
    }

    template <typename T> void write(const T& value) const
    {
        static_assert(std::is_trivially_copyable_v<T>);
        transfer(const_cast<T*>(&value), sizeof(value), true);
    }
    template <typename T> void read(T& value) const
    {
        static_assert(std::is_trivially_copyable_v<T>);
        transfer(&value, sizeof(value), false);
    }

    int descriptor;
    std::function<void()> tick = [] {};
};

struct Process
{
    explicit Process(int descriptor, const std::string& executable)
    {
        const auto argument = std::to_string(descriptor);
        char* arguments[] = {const_cast<char*>(executable.c_str()),
                             const_cast<char*>("--python-worker"),
                             const_cast<char*>(argument.c_str()), nullptr};
        posix_spawn_file_actions_t actions;
        int error = posix_spawn_file_actions_init(&actions);
        if (error)
            throw std::system_error(error, std::generic_category(), "Python worker actions");
        posix_spawnattr_t attributes;
        error = posix_spawnattr_init(&attributes);
        if (!error)
        {
            error = posix_spawn_file_actions_adddup2(&actions, descriptor, descriptor);
            if (!error)
                error = posix_spawnattr_setflags(&attributes, POSIX_SPAWN_SETPGROUP);
            if (!error)
                error = posix_spawnattr_setpgroup(&attributes, 0);
            if (!error)
                error = posix_spawn(&pid, executable.c_str(), &actions, &attributes, arguments,
                                    environ);
            posix_spawnattr_destroy(&attributes);
        }
        posix_spawn_file_actions_destroy(&actions);
        if (error)
            throw std::system_error(error, std::generic_category(), "Cannot start Python worker");
    }
    ~Process()
    {
        if (pid > 0)
        {
            kill(-pid, SIGKILL);
            kill(pid, SIGKILL);
            while (waitpid(pid, nullptr, 0) < 0 && errno == EINTR)
            {
            }
        }
    }
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    void wait(const std::function<void()>& tick)
    {
        while (true)
        {
            tick();
            siginfo_t result{};
            if (waitid(P_PID, pid, &result, WEXITED | WNOHANG | WNOWAIT) < 0)
            {
                if (errno == EINTR)
                    continue;
                throw std::system_error(errno, std::generic_category(), "Python worker wait");
            }
            if (result.si_pid != 0)
            {
                // Kill descendants before reaping the leader, whose PID identifies its group.
                kill(-pid, SIGKILL);
                kill(pid, SIGKILL);
                while (waitpid(pid, nullptr, 0) < 0 && errno == EINTR)
                {
                }
                pid = -1;
                if (result.si_code != CLD_EXITED || result.si_status != 0)
                    throw std::runtime_error("Python worker exited abnormally");
                return;
            }
            std::this_thread::sleep_for(10ms);
        }
    }

    pid_t pid{-1};
};

Channel* workerChannel = nullptr;
std::mutex requestMutex;

struct ReleaseGIL
{
    ReleaseGIL() : state(PyEval_SaveThread()) {}
    ~ReleaseGIL()
    {
        PyEval_RestoreThread(state);
    }
    PyThreadState* state;
};
#endif
} // namespace

PythonMessage ofsim_python_integration::exchangePythonMessage(PythonMessage message, u64 runId)
{
#ifdef __linux__
    if (workerChannel == nullptr)
        throw std::runtime_error("Python API requires a worker process");

    // Release before locking: another Python thread may already be awaiting its reply.
    ReleaseGIL release;
    std::lock_guard<std::mutex> lock(requestMutex);
    const auto operation = message.operation;
    workerChannel->write(message);
    workerChannel->read(message);
    message.error[sizeof(message.error) - 1] = '\0';
    if (message.operation != operation && message.operation != PythonOperation::ERROR)
        throw std::runtime_error("Invalid Python worker reply");
#else
    message = handleRequest(message, runId);
#endif
    if (message.operation == PythonOperation::ERROR)
        throw std::runtime_error(message.error);
    return message;
}

void ofsim_python_integration::pythonWorkerReady()
{
#ifdef __linux__
    PythonMessage message;
    message.operation = PythonOperation::READY;
    workerChannel->write(message);
#endif
}

void PythonMachine::runPythonOrbitalProgram(std::string sourceCode, u64 runId)
{
#ifdef __linux__
    if (sourceCode.size() > maxSourceSize)
        throw std::runtime_error("Python source exceeds the 4 MiB limit");
    const auto executable =
        workerExecutable.empty()
            ? (std::filesystem::read_symlink("/proc/self/exe").parent_path() / "OFSim").string()
            : workerExecutable;
    int descriptors[2];
    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0, descriptors) < 0)
        throw std::system_error(errno, std::generic_category(), "Python worker channel");
    Channel channel(descriptors[0]);
    Channel childChannel(descriptors[1]);
    Process process(childChannel.descriptor, executable);
    // The parent's copy must close now so worker crashes produce EOF.
    close(childChannel.descriptor);
    childChannel.descriptor = -1;

    const auto started = Clock::now();
    bool ready = false;
    std::optional<Clock::time_point> stopping;
    channel.tick = [&]
    {
        const auto now = Clock::now();
        if (ofsim_events::EventProcessor::getInstance()->isPythonMachineTerminated() && !stopping)
            stopping = now;
        if (stopping && now - *stopping >= stopTimeout)
            throw Stopped{};
        if (!ready && now - started >= startupTimeout)
            throw std::runtime_error("Python worker startup timed out");
    };

    try
    {
        channel.write(static_cast<u32>(sourceCode.size()));
        channel.transfer(sourceCode.data(), sourceCode.size(), true);
        while (true)
        {
            PythonMessage message;
            channel.read(message);
            if (message.operation == PythonOperation::READY && !ready)
            {
                ready = true;
                continue;
            }
            if (message.operation == PythonOperation::SUCCESS ||
                message.operation == PythonOperation::ERROR)
            {
                const auto finished = Clock::now();
                process.wait(
                    [&]
                    {
                        channel.tick();
                        if (Clock::now() - finished >= stopTimeout)
                            throw std::runtime_error("Python worker exit timed out");
                    });
                if (message.operation == PythonOperation::ERROR)
                {
                    message.error[sizeof(message.error) - 1] = '\0';
                    throw std::runtime_error(message.error);
                }
                return;
            }
            if (!ready || (message.operation != PythonOperation::TELEMETRY &&
                           message.operation != PythonOperation::THRUST &&
                           message.operation != PythonOperation::ROTATION))
                throw std::runtime_error("Invalid Python worker message");
            channel.write(handleRequest(message, runId));
        }
    }
    catch (const Stopped&)
    {
        // Process cleanup forcibly stops the worker and its remaining descendants.
    }
#else
    runEmbedded(std::move(sourceCode), runId);
#endif
}

int PythonMachine::runWorker(int descriptor)
{
#ifdef __linux__
    const auto parent = getppid();
    if (fcntl(descriptor, F_SETFD, FD_CLOEXEC) < 0 || prctl(PR_SET_PDEATHSIG, SIGKILL) < 0 ||
        getppid() != parent)
        return 2;
    Channel channel(descriptor);
    workerChannel = &channel;
    try
    {
        u32 size;
        channel.read(size);
        if (size > maxSourceSize)
            return 2;
        std::string source(size, '\0');
        channel.transfer(source.data(), source.size(), false);
        PythonMessage result;
        try
        {
            runEmbedded(std::move(source), 0);
        }
        catch (const std::exception& error)
        {
            result.operation = PythonOperation::ERROR;
            std::snprintf(result.error, sizeof(result.error), "%s", error.what());
        }
        channel.write(result);
        workerChannel = nullptr;
        return 0;
    }
    catch (const std::exception&)
    {
        return 2;
    }
#else
    return 2;
#endif
}
