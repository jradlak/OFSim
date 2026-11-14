#pragma once

#include <chrono>
#include <exception>
#include <functional>
#include <future>
#include <stdexcept>
#include <thread>
#include <utility>

namespace ofsim_simulation
{

// Owned by the UI thread. Work and its stop callback must keep their targets
// alive until poll() has collected the worker.
class ProgramRunner
{
public:
    ~ProgramRunner()
    {
        // Fallback for unwinding; normal shutdown polls while servicing the window.
        requestStop();
        if (worker.joinable())
            worker.join();
    }

    void start(std::function<void()> work, std::function<void()> stop)
    {
        if (active())
            throw std::logic_error("A flight program is already active");

        std::packaged_task<void()> task(std::move(work));
        auto result = task.get_future();
        worker =
            std::thread([task = std::move(task)]() mutable { task.make_ready_at_thread_exit(); });
        completion = std::move(result);
        stopCallback = std::move(stop);
        stopping = false;
        failure = nullptr;
    }

    void requestStop()
    {
        if (!active() || stopping)
            return;
        stopping = true;
        stopCallback();
    }

    // Never waits for a running program. Exceptions are delivered on the UI thread.
    bool poll()
    {
        if (!active() || completion.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
            return false;

        worker.join();
        try
        {
            completion.get();
        }
        catch (...)
        {
            failure = std::current_exception();
        }
        stopCallback = {};
        stopping = false;
        return true;
    }

    bool active() const
    {
        return worker.joinable();
    }
    std::exception_ptr error() const
    {
        return failure;
    }

private:
    std::thread worker;
    std::future<void> completion;
    std::function<void()> stopCallback;
    bool stopping{false};
    std::exception_ptr failure;
};

} // namespace ofsim_simulation
