#include "../i18n.h"

std::string i18n::translate(Label label, Language lang)
{
	return std::string();
}

void i18n::initLabels()
{
	plLabels[menu_new] = "Nowy";
	enLabels[menu_new] = "New";
	plLabels[menu_open] = "Otworz...";
	enLabels[menu_open] = "Open...";
	plLabels[menu_save] = "Zapisz...";
	enLabels[menu_save] = "Save..";
	plLabels[menu_close] = "Zamknij";
	enLabels[menu_close] = "Close";
	plLabels[menu_view] = "Widok";
	enLabels[menu_view] = "View";
	plLabels[menu_telemetry_data] = "Telemetria - dane";
	enLabels[menu_telemetry_data] = "Telemetry - data";
	plLabels[menu_telemetry_plots] = "Telemetria - wykresy";
	enLabels[menu_telemetry_plots] = "Telemetry - plots";
	plLabels[menu_command_history] = "Historia komend";
	enLabels[menu_command_history] = "Command history";
	plLabels[menu_program_source_code] = "Kod zrodlowy programu lotu";
	enLabels[menu_program_source_code] = "Flight program source code";
	plLabels[menu_manual] = "Instrukcja obs³ugi";
	enLabels[menu_manual] = "Operation manual";
	plLabels[menu_about] = "O programie...";
	enLabels[menu_about] = "About...";

	/*
	menu_new,
	menu_open,
	menu_save,
	menu_close,
	menu_view,
	menu_telemetry_data,
	menu_telemetry_plots,
	menu_command_history,
	menu_program_source_code,
	menu_manual,
	menu_about,

	splash_info,
	splash_version,
	splash_author,

	dialog_title,
	dialog_directory,
	dialog_load,
	dialog_file_list,
	dialog_cancel,

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
	commands_thrust_rotation_z
	*/

	//plLabels[]
}
