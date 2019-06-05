# Create a function to write to the data bus at
# a given address.
proc writeBus {addressValue busValue} {
force -freeze address 16#$addressValue
force -freeze writedata 16#$busValue
force -freeze write 1
run 125000
force -freeze write 0
# Release the data and address buses.
noforce writedata
noforce address
run 125000
}

# Create a function to read from an register to the data bus at
# a given address.
proc readBus {addressValue} {
force -freeze address 16#$addressValue
force -freeze write 0
force -freeze read 1
run 125000
run 125000
force -freeze read 0
# Release the address bus.
noforce readdata
noforce address
run 125000
}

proc runSim {} {
    restart -force -nowave
    
    add wave *
    
    property wave -radix hex *
    
    force -deposit clk 1 0, 0 {62500} -repeat 125000
    
    force -freeze reset 0
    run 2000
    
    force -freeze reset 1
   
   # writeBus 02 00000226
    #writeBus 03 00000001
    #writeBus 00 00000002
    #run 25000
    
    force -freeze data_bus_in 16#00000080
    readBus 04
    noforce data_bus_in
    run 
    run 125000
}