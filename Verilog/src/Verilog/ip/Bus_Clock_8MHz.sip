set_global_assignment -entity "Bus_Clock_8MHz" -library "lib_Bus_Clock_8MHz" -name IP_TOOL_NAME "altera_pll"
set_global_assignment -entity "Bus_Clock_8MHz" -library "lib_Bus_Clock_8MHz" -name IP_TOOL_VERSION "18.1"
set_global_assignment -entity "Bus_Clock_8MHz" -library "lib_Bus_Clock_8MHz" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_Bus_Clock_8MHz" -name SPD_FILE [file join $::quartus(sip_path) "Bus_Clock_8MHz.spd"]

set_global_assignment -library "lib_Bus_Clock_8MHz" -name MISC_FILE [file join $::quartus(sip_path) "Bus_Clock_8MHz_sim/Bus_Clock_8MHz.vo"]
