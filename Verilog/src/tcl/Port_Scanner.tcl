proc runSim {} {
    restart -force -nowave
    
    add wave *
    
    property wave -radix hex *
    
    force -deposit sys_clock 1 0, 0 {20ns} -repeat 40000
    
    force -freeze reset 1
    run 60ns
    force -freeze reset 0
    run 60ns
    
    force -freeze data_in 0000000010000000
    run 1000ns
    
    force -freeze data_in 16#AA
    run 2000000ns
}