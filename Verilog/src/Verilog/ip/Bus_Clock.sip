set_global_assignment -entity "Bus_Clock" -library "lib_Bus_Clock" -name IP_TOOL_NAME "altera_pll"
set_global_assignment -entity "Bus_Clock" -library "lib_Bus_Clock" -name IP_TOOL_VERSION "18.1"
set_global_assignment -entity "Bus_Clock" -library "lib_Bus_Clock" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_Bus_Clock" -name SPD_FILE [file join $::quartus(sip_path) "Bus_Clock.spd"]

set_global_assignment -library "lib_Bus_Clock" -name MISC_FILE [file join $::quartus(sip_path) "Bus_Clock_sim/Bus_Clock.vo"]
