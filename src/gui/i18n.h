#pragma once

#include <string>
#include <map>

enum Language {
    PL,
    EN
};

enum Label {
	menu_new,
	menu_open,
	menu_save,
	menu_save_as,
	menu_close,
	menu_view,
	menu_telemetry_data,
	menu_telemetry_plots,
	menu_command_history,
	menu_program_source_code,
	menu_help,
	menu_manual,
	menu_about,

	splash_title,
	splash_info,
	splash_version,
	splash_author,

	dialog_title,
	dialog_directory,
	dialog_load,
	dialog_file_list,
	dialog_cancel,

	dialog_save_title,
	dialog_save_file_name,

	sim_panel_title,
	sim_panel_clock,
	sim_panel_time_compression,

	code_editor_title,

	telemetry_title,
	telemetry_altitude,
	telemetry_mass,
	telemetry_atm_press,
	telemetry_velocity,
	telemetry_position,
	telemetry_perigee,
	telemetry_apogee,

	telemetry_plots_title,
	telemetry_plots_velocity,
	telemetry_plots_delta_v,
	telemetry_plots_altitude,
	telemetry_plots_atm_press,

	presentation_title,
	presentation_distance,

	commands_title,
	commands_thrust_change,
	commands_thrust_direction_x,
	commands_thrust_direction_y,
	commands_thrust_direction_z,
	commands_thrust_rotation_x,
	commands_thrust_rotation_y,
	commands_thrust_rotation_z,

	translate_error
};

class I18n
{
public:
	
	static I18n* getInstance();

	const char* t(Label label);
	const char* translate(Label label, Language lang);
	void switchLanguage(Language newLanguage) { currentLanguage = newLanguage; }

	~I18n() {}

private:
	I18n()
	{
		initLabels();
		currentLanguage = PL;
	}

	I18n(const I18n&) = delete;
	I18n& operator=(const I18n&) = delete;

	std::map<Label, std::string> plLabels;
	std::map<Label, std::string> enLabels;

	Language currentLanguage;

	void initLabels();
};