proc runSim {} {
    restart -force -nowave
    
    add wave *
    
    property wave -radix hex *
    
    force -deposit clk_50MHz 1 0, 0 {20ns} -repeat 40000
    force -deposit bus_clock 1 0, 0 {125ns} -repeat 250000
    
    force -freeze SW0 0
    run 60ns
    force -freeze SW0 1
    run 60ns
     
    force -freeze data_in 16#AA
    run 2000000ns
}