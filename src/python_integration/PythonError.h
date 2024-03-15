#pragma once
#include <string>

namespace ofsim_python_integration
{
    struct PythonError
    {
        bool active { false };
        std::string metadata { "" };
        std::string message { "" };
    };
}