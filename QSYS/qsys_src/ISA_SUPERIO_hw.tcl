# TCL File Generated by Component Editor 18.1
# Sat Jun 01 00:15:48 EST 2019
# DO NOT MODIFY


# 
# ISA_SUPERIO "ISA_SUPERIO" v0.2
# Matthew Atkin & Jesse Buhagiar 2019.06.01.00:15:48
# 
# 

# 
# request TCL package from ACDS 16.1
# 
package require -exact qsys 16.1


# 
# module ISA_SUPERIO
# 
set_module_property DESCRIPTION ""
set_module_property NAME ISA_SUPERIO
set_module_property VERSION 0.2
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR "Matthew Atkin & Jesse Buhagiar"
set_module_property DISPLAY_NAME ISA_SUPERIO
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL SuperIO
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file Address_Decoder.v VERILOG PATH ../superio_src/Verilog/Address_Decoder.v
add_fileset_file Bus_Interface.v VERILOG PATH ../superio_src/Verilog/Bus_Interface.v
add_fileset_file Mux_4_To_1.v VERILOG PATH ../superio_src/Verilog/Mux_4_To_1.v
add_fileset_file Register.v VERILOG PATH ../superio_src/Verilog/Register.v
add_fileset_file RegisterRW.v VERILOG PATH ../superio_src/Verilog/RegisterRW.v
add_fileset_file Register_File.v VERILOG PATH ../superio_src/Verilog/Register_File.v
add_fileset_file State_Machine.v VERILOG PATH ../superio_src/Verilog/State_Machine.v
add_fileset_file SuperIO.v VERILOG PATH ../superio_src/Verilog/SuperIO.v TOP_LEVEL_FILE


# 
# parameters
# 


# 
# display items
# 


# 
# connection point avalon_slave_0
# 
add_interface avalon_slave_0 avalon end
set_interface_property avalon_slave_0 addressUnits WORDS
set_interface_property avalon_slave_0 associatedClock clock
set_interface_property avalon_slave_0 associatedReset reset
set_interface_property avalon_slave_0 bitsPerSymbol 8
set_interface_property avalon_slave_0 burstOnBurstBoundariesOnly false
set_interface_property avalon_slave_0 burstcountUnits WORDS
set_interface_property avalon_slave_0 explicitAddressSpan 0
set_interface_property avalon_slave_0 holdTime 0
set_interface_property avalon_slave_0 linewrapBursts false
set_interface_property avalon_slave_0 maximumPendingReadTransactions 0
set_interface_property avalon_slave_0 maximumPendingWriteTransactions 0
set_interface_property avalon_slave_0 readLatency 0
set_interface_property avalon_slave_0 readWaitTime 1
set_interface_property avalon_slave_0 setupTime 0
set_interface_property avalon_slave_0 timingUnits Cycles
set_interface_property avalon_slave_0 writeWaitTime 0
set_interface_property avalon_slave_0 ENABLED true
set_interface_property avalon_slave_0 EXPORT_OF ""
set_interface_property avalon_slave_0 PORT_NAME_MAP ""
set_interface_property avalon_slave_0 CMSIS_SVD_VARIABLES ""
set_interface_property avalon_slave_0 SVD_ADDRESS_GROUP ""

add_interface_port avalon_slave_0 write write Input 1
add_interface_port avalon_slave_0 read read Input 1
add_interface_port avalon_slave_0 writedata writedata Input 32
add_interface_port avalon_slave_0 readdata readdata Output 32
add_interface_port avalon_slave_0 address address Input 3
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isFlash 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isPrintableDevice 0


# 
# connection point reset
# 
add_interface reset reset end
set_interface_property reset associatedClock clock
set_interface_property reset synchronousEdges DEASSERT
set_interface_property reset ENABLED true
set_interface_property reset EXPORT_OF ""
set_interface_property reset PORT_NAME_MAP ""
set_interface_property reset CMSIS_SVD_VARIABLES ""
set_interface_property reset SVD_ADDRESS_GROUP ""

add_interface_port reset global_reset reset_n Input 1


# 
# connection point clock
# 
add_interface clock clock end
set_interface_property clock clockRate 0
set_interface_property clock ENABLED true
set_interface_property clock EXPORT_OF ""
set_interface_property clock PORT_NAME_MAP ""
set_interface_property clock CMSIS_SVD_VARIABLES ""
set_interface_property clock SVD_ADDRESS_GROUP ""

add_interface_port clock clk_50MHz clk Input 1


# 
# connection point conduit_end
# 
add_interface conduit_end conduit end
set_interface_property conduit_end associatedClock clock
set_interface_property conduit_end associatedReset reset
set_interface_property conduit_end ENABLED true
set_interface_property conduit_end EXPORT_OF ""
set_interface_property conduit_end PORT_NAME_MAP ""
set_interface_property conduit_end CMSIS_SVD_VARIABLES ""
set_interface_property conduit_end SVD_ADDRESS_GROUP ""

add_interface_port conduit_end A export_address Output 16
add_interface_port conduit_end AEN export_aen Output 1
add_interface_port conduit_end D export_data Bidir 16
add_interface_port conduit_end DACK1 export_dack1 Output 1
add_interface_port conduit_end DACK3 export_dack3 Output 1
add_interface_port conduit_end DACK5 export_dack5 Output 1
add_interface_port conduit_end DACK7 export_dack7 Output 1
add_interface_port conduit_end DRQ1 export_drq1 Input 1
add_interface_port conduit_end DRQ3 export_drq3 Input 1
add_interface_port conduit_end DRQ5 export_drq5 Input 1
add_interface_port conduit_end DRQ7 export_drq7 Input 1
add_interface_port conduit_end IOR export_ior Output 1
add_interface_port conduit_end IOW export_iow Output 1
add_interface_port conduit_end IRQ2 export_irq2 Input 1
add_interface_port conduit_end IRQ5 export_irq5 Input 1
add_interface_port conduit_end IRQ7 export_irq7 Input 1
add_interface_port conduit_end IRQ10 export_irq10 Input 1
add_interface_port conduit_end RESET export_reset Bidir 1

