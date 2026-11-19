#pragma once

#include "../infrastructure/EventProcessor.h"

namespace ofsim_python_integration
{
enum class PythonOperation
{
    READY,
    TELEMETRY,
    THRUST,
    ROTATION,
    SUCCESS,
    ERROR
};

// Private protocol between two instances of the same executable.
struct PythonMessage
{
    PythonOperation operation{PythonOperation::SUCCESS};
    ofsim_events::TelemetrySnapshot telemetry{};
    dvec3 value{0.0};
    bool shouldStop{false};
    char error[1024]{};
};

PythonMessage exchangePythonMessage(PythonMessage message, u64 runId);
void pythonWorkerReady();
} // namespace ofsim_python_integration
