# Create a function to write to the data bus at
# a given address.
proc writeBus {addressValue busValue} {
force -freeze address 16#$addressValue
force -freeze writedata 16#$busValue
force -freeze write 1
run 250000
force -freeze write 0
# Release the data and address buses.
noforce writedata
noforce address
run 250000
}

# Create a function to read from an register to the data bus at
# a given address.
proc readBus {addressValue} {
force -freeze address 16#$addressValue
force -freeze write 0
force -freeze read 1
run 250000
run 250000
force -freeze read 0
# Release the address bus.
noforce readdata
noforce address
run 250000
}

proc runSim {} {
    restart -force -nowave
    
    add wave *
    
    property wave -radix hex *
    
    force -deposit clk_bus 1 0, 0 {125ns} -repeat 250000
    
    force -freeze global_reset 0
    run 2000
    
    force -freeze global_reset 1
    
    writeBus 02 00000226
    writeBus 03 00000001
    writeBus 00 00000002
    run 2500000
    
    writeBus 03 00000000
    writeBus 00 00000002
    run 2500000
    
    writeBus 02 0000022E
    writeBus 00 00000001
    force -freeze D 16#00000080
    run 2500000
    readBus 04 
    noforce D

    writeBus 02 0000022A
    writeBus 00 00000001
    force -freeze D 16#000000AA
    run 2500000
    readBus 04
    noforce D
    
    run 3000000
}