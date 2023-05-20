#include "../I18n.h"
#include <iostream>

I18n* I18n::getInstance()
{
	static I18n* i18n;

	if (i18n == nullptr)
	{
		i18n = new I18n();
	}
	
	return i18n;
}

const char* I18n::t(Label label)
{	
	return translate(label, currentLanguage);
}

const char* I18n::translate(Label label, Language lang)
{
	if (lang == PL) {
		return (plLabels[label]).c_str();
	}
	else 
	{
		return (enLabels[label]).c_str();
	}
}

void I18n::initLabels()
{
	plLabels[menu_new] = "Nowy";
	enLabels[menu_new] = "New";
	plLabels[menu_open] = "Otworz...";
	enLabels[menu_open] = "Open...";
	plLabels[menu_save] = "Zapisz";
	enLabels[menu_save] = "Save";
	plLabels[menu_save_as] = "Zapisz jako...";
	enLabels[menu_save_as] = "Save as...";
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
	plLabels[menu_help] = "Pomoc";
	enLabels[menu_help] = "Help";
	plLabels[menu_manual] = "Instrukcja obs�ugi";
	enLabels[menu_manual] = "Operation manual";
	plLabels[menu_about] = "O programie...";
	enLabels[menu_about] = "About...";

	plLabels[splash_info] = "Symulator Lot�w Orbitalnych";
	enLabels[splash_info] = "Orbital Flight Simulator";
	plLabels[splash_version] = "Wersja: 0.21.2 alpha";
	enLabels[splash_version] = "Version: 0.21.2 alpha";

	plLabels[dialog_title] = "Wczytaj kod zrodlowy programu lotu";
	enLabels[dialog_title] = "Load the source code of the flight program";
	plLabels[dialog_directory] = "Katalog: ";
	enLabels[dialog_directory] = "Directory: ";
	plLabels[dialog_load] = "Wczytaj";
	enLabels[dialog_load] = "Load";
	plLabels[dialog_file_list] = "Lista plik�w: ";
	enLabels[dialog_file_list] = "List of files: ";
	plLabels[dialog_cancel] = "Anuluj";
	enLabels[dialog_cancel] = "Cancel";

	plLabels[dialog_save_title] = "Zapisz plik jako";
	enLabels[dialog_save_title] = "Save file as";
	plLabels[dialog_save_file_name] = "Pelna nazwa pliku: ";
	enLabels[dialog_save_file_name] = "Full file name: ";

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
	plLabels[telemetry_altitude] = "Wysoko�� punktu widzenia: ";
	enLabels[telemetry_altitude] = "Altitude: ";	
	plLabels[telemetry_mass] = "Masa rakiety: ";
	enLabels[telemetry_mass] = "Mass of the rocket: ";
	plLabels[telemetry_atm_press] = "Cisnienie dynamiczne atmosfery: ";
	enLabels[telemetry_atm_press] = "Dynamic atmospheric pressure: ";
	plLabels[telemetry_velocity] = "Predkosc rakiety: ";
	enLabels[telemetry_velocity] = "Velocity: ";
	plLabels[telemetry_position] = "Pozycja rakiety: ";
	enLabels[telemetry_position] = "Position: ";
	plLabels[telemetry_perigee] = "Perygeum: ";
	enLabels[telemetry_perigee] = "Perigee: ";
	plLabels[telemetry_apogee] = "Apogeum: ";
	enLabels[telemetry_apogee] = "Apogee: ";

	plLabels[telemetry_plots_title] = "Wykresy telemetrii";
	enLabels[telemetry_plots_title] = "Telemetry plots";
	plLabels[telemetry_plots_velocity] = "Predkosc";
	enLabels[telemetry_plots_velocity] = "Velocity";
	plLabels[telemetry_plots_delta_v] = "Delta-v";
	enLabels[telemetry_plots_delta_v] = "Delta-v";
	plLabels[telemetry_plots_altitude] = "Wysokosc";
	enLabels[telemetry_plots_altitude] = "Altitude";
	plLabels[telemetry_plots_atm_press] = "Cisn. atm.";
	enLabels[telemetry_plots_atm_press] = "Atm. pressure";

	plLabels[presentation_title] = "Tryb prezentacji - podroz na Ksiezyc";
	enLabels[presentation_title] = "Presentation mode - a journey to the Moon";
	plLabels[presentation_distance] = "Odleglosc od rakiety: ";
	enLabels[presentation_distance] = "Distance form the rocket: ";	

	plLabels[commands_title] = "Wykonane komendy";
	enLabels[commands_title] = "Executed commands";

	plLabels[commands_thrust_change] = "zmiana wartosci ciagu: ";
	enLabels[commands_thrust_change] = "thrust magnitude change: ";
	plLabels[commands_thrust_direction_x] = "zmiana kierunku ciagu w osi X: ";
	enLabels[commands_thrust_direction_x] = "thrust direction X change: ";
	plLabels[commands_thrust_direction_y] = "zmiana kierunku ciagu w osi Y: ";
	enLabels[commands_thrust_direction_y] = "thrust direction Y change: ";
	plLabels[commands_thrust_direction_z] = "zmiana kierunku ciagu w osi Z: ";
	enLabels[commands_thrust_direction_z] = "thrust direction Z change: ";
	plLabels[commands_thrust_rotation_x] = "zmiana rotacji zyroskopowej w osi X: ";
	enLabels[commands_thrust_rotation_x] = "change gyroscopic rotation in X axis: ";
	plLabels[commands_thrust_rotation_y] = "zmiana rotacji zyroskopowej w osi Y: ";
	enLabels[commands_thrust_rotation_y] = "change gyroscopic rotation in Y axis: ";
	plLabels[commands_thrust_rotation_z] = "zmiana rotacji zyroskopowej w osi Z: ";
	enLabels[commands_thrust_rotation_z] = "change gyroscopic rotation in Z axis: ";

	plLabels[splash_title] = "Symulator Lotow Orbitalnych";
	enLabels[splash_title] = "Orbital Flight Simulator";
	plLabels[splash_info] =
		"Idea programu jest symulowanie lotow balistycznych (w tym orbitalnych) rakiety w polu grawitacyjnym planety o parametrach zblizonych do ziemskich."
		" Rakieta jest kierowana przez specjalnie zaprojektowany komputer wirtualny (maszyne wirtualna)."
		" Sterowanie rakieta polega na pisaniu programow w jezyku asemblera maszyny wirtualnej."
		" Program asemblera ma dostep do pamieci komputera, w ktorej przechowywane sa dane telemetryczne lotu rakiety"
		" Na podstawie tych danych program moze wydawac rakiecie komendy takie jak: zmiana kierunku i dlugosci wektora ciagu,"
		" sterowanie zestawem zyroskopow obrotowych na orbicie, itp.";

	enLabels[splash_info] = "The idea of the program is to simulate ballistic flights (including orbital) of a rocket in the gravitational field of a planet with parameters similar to Earth."
		"The rocket is controlled by a specially designed virtual computer(virtual machine)."
		"Rocket control involves writing programs in the assembly language of the virtual machine."
		"The assembly program has access to the computer's memory, where telemetry data of the rocket's flight is stored.Based on this data,"
		" the program can issue commands to the rocket, such as changing the direction and length of the thrust vector, controlling a set of rotating gyroscopes in orbit, etc.";	

	plLabels[translate_error] = "Blad translacji w linii @lineNumber, nierozpoznana instrukcja: @lineTxt";
	enLabels[translate_error] = "Translation error in line: @lineNumber, unrecognized instruction: @lineTxt";

}
