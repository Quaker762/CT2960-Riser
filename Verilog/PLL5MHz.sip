set_global_assignment -entity "PLL5MHz" -library "lib_PLL5MHz" -name IP_TOOL_NAME "altera_pll"
set_global_assignment -entity "PLL5MHz" -library "lib_PLL5MHz" -name IP_TOOL_VERSION "18.1"
set_global_assignment -entity "PLL5MHz" -library "lib_PLL5MHz" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_PLL5MHz" -name SPD_FILE [file join $::quartus(sip_path) "PLL5MHz.spd"]

set_global_assignment -library "lib_PLL5MHz" -name MISC_FILE [file join $::quartus(sip_path) "PLL5MHz_sim/PLL5MHz.vo"]
