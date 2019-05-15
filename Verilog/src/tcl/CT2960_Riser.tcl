proc runSim {} {
    restart -force -nowave
    
    add wave *
    
    property wave -radix hex *
    
    force -deposit clk_5MHz 1 0, 0 {200ns} -repeat 400000
    
    force -freeze set_safe 1
    run 400ns
    
    force -freeze set_safe 0
    run 8000ns
    
    force -freeze data_in 0000000010000000
    run 1000ns
    
    force -freeze data_in 16#AA
    run 1000ns
}