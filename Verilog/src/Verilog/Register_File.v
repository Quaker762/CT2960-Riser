module Register_File
(
    input           write,
    input           read,
    input   [31:0]  writedata,
    input   [31:0]  readdata,
    input   [2:0]   address,
    
    output  [15:0]  data_out,
    output  [15:0]  address_out,
    output  [7:0]   control_out
);

Regiset #(32) address_reg
(
    .D(),
    .clk(),
    .reset(),
    .load(),
    
    .Q(address_out)
);

Register #(32) data_reg
(
    .D(),
    .clk(),
    .reset(),
    .load(),
    
    .Q(data_out)
);

Register #(32) control_reg
(
    .D(),
    .clk(),
    .reset(),
    .load(),
    
    .Q(control_out)
);

endmodule
