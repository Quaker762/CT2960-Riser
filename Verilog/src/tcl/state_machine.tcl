proc runSim {} {
    restart -force -nowave
    
    add wave *
    
    property wave -radix hex *
    
    force -deposit clk 1 0, 0 {62500} -repeat 125000
    
    force -freeze reset 0
    run 2000
    
    force -freeze reset 1
    
    force -freeze control_in 16#2
    run 600000
    
    force -freeze reset 0
    run 20000
    
    force -freeze reset 1
    run 400000
}