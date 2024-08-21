#pragma once

#include "../math_and_physics/MathTypes.h"

namespace ofsim_events
{
	enum class UserAction
	{
		FILE_NEW,
		FILE_OPEN,

		PROGRAM_FILE_OPEN,
		PROGRAM_TRANSLATE,
		PYTHON_PROGRAM_RAISED_ERROR,
		PYTHON_PROGRAM_EXECUTION_STOP,

		VM_PROGRAM_FILE_OPEN,
		VM_PROGRAM_TRANSLATE,
		VM_PROGRAM_RAISED_ERROR,
		VM_PROGRAM_EXECUTION_STOP,

		FILE_SAVE,
		FILE_SAVED_AS,
		FILE_EXIT,

		VIEW_TELEMETRY,
		VIEW_TELEMETRY_PLOT,
		VIEW_COMMANDS,
		VIEW_PROGRAM,

		HELP_ABOUT,
		HELP_HELP,

		CHANGE_MODE_TO_FROM_PREDICTION,
		CHANGE_MODE_TO_FORM_PRESENTATION,

		CHANGE_MODE_TO_FROM_DIAGNOSTICS,

		ROTATION_LONGITUDE_UP,
		ROTATION_LONGITUDE_DOWN,
		ROTATION_LATITUDE_UP,
		ROTATION_LATITUDE_DOWN,

		SHOW_DIAGNOSTICS_IN_SIMULATION,

		NONE
	};

	// This kind of event is producend by the user interface or by te Ptyhon machine when scriupt error occurs.
	struct SimulationEvent
	{
		u32 id{ 0 };
		u64 timestamp{ 0 };
		UserAction action;
		std::string data;

		SimulationEvent() : action(UserAction::NONE) {}
		SimulationEvent(u32 _id, u64 _timestamp, UserAction _action, std::string _data)
			: id(_id), timestamp(_timestamp), action(_action), data(_data) {}
	};
}