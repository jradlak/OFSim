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
	plLabels[splash_info] = "Symulator Lotów Orbitalnych";
	enLabels[splash_info] = "Orbital Flight Simulator";
	plLabels[splash_version] = "Wersja: 0.21.1 alpha";
	enLabels[splash_version] = "Version: 0.21.1 alpha";
	plLabels[dialog_title] = "Wczytaj kod zrodlowy programu lotu";
	enLabels[dialog_title] = "Load the source code of the flight program";
	plLabels[dialog_directory] = "Katalog: ";
	enLabels[dialog_directory] = "Directory: ";
	plLabels[dialog_load] = "Wczytaj";
	enLabels[dialog_load] = "Load";
	plLabels[dialog_file_list] = "Lista plików: ";
	enLabels[dialog_file_list] = "List of files: ";
	plLabels[dialog_cancel] = "Anuluj";
	enLabels[dialog_cancel] = "Cancel";
	plLabels[sim_panel_title] = "Kontrola symulacji";
	enLabels[sim_panel_title] = "Simulation control panel";
	plLabels[sim_panel_clock] = "Zegar: ";
	enLabels[sim_panel_clock] = "Clock: ";
	plLabels[sim_panel_time_compression] = "Kompresja czasu: ";
	enLabels[sim_panel_time_compression] = "Time compression: ";
	plLabels[code_editor_title] = "Edytor kodu zrodlowego programu lotu";
	enLabels[code_editor_title] = "Flight program source code editor";
	plLabels[telemetry_title] = "Telemetria";
	enLabels[telemetry_title] = "Telemetry";
	plLabels[telemetry_altitude] = "Wysokoœæ punktu widzenia: ";
	enLabels[telemetry_altitude] = "Altitude: ";	
	plLabels[telemetry_mass] = "Masa rakiety: ";
	enLabels[telemetry_mass] = "Mass of the rocket: ";
	plLabels[telemetry_atm_press] = "Cisnienie dynamiczne atmosfery: ";
	enLabels[telemetry_atm_press] = "Dynamic atmospheric pressure: ";

	/*
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
