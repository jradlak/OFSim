#include "simulation/Simulation.h"

#include <charconv>
#include <string_view>

using namespace ofsim_simulation;

int main(int argc, char** argv)
{
    if (argc == 3 && std::string_view(argv[1]) == "--python-worker")
    {
        int descriptor;
        const std::string_view argument(argv[2]);
        const auto result =
            std::from_chars(argument.data(), argument.data() + argument.size(), descriptor);
        if (result.ec != std::errc{} || result.ptr != argument.data() + argument.size() ||
            descriptor < 0)
            return 2;
        return ofsim_python_integration::PythonMachine::runWorker(descriptor);
    }

    Simulation simulation;
    simulation.start();
}
