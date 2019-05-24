module Register_File
(
    input           write,
    input           read,
    input   [31:0]  writedata,
    output  [31:0]  readdata,
    input   [2:0]   address,
    input           clk,
    
    input           control_reset,
    
    output  [15:0]  data_out,
    output  [15:0]  address_out,
    output  [7:0]   control_out
);

wire [3:0] decodedAddr;

wire control_reg_load	= decodedAddr[0];
wire addr_reg_load 		= decodedAddr[1];
wire data_reg_load_w	= decodedAddr[2];
wire data_reg_load_r    = decodedAddr[3];

Address_Decoder decoder
(
	.ce(write),
	.address(address),
	.out(decodedAddr)
);

Register #(32) address_reg
(
    .D(writeData),
    .clk(clk),
    .reset(control_reset),
    .load(addr_reg_load),
    
    .Q(address_out)
);

// This needs to be a 2 input Flip flop!
RegisterRW #(32) data_reg
(
    .D(writeData),
	.D2(readData),
    .clk(clk),
    .reset(control_reset),
    .load(data_reg_load_w),
	.load2(data_reg_load_r),
    
    .Q(data_out)
);

Register #(32) control_reg
(
    .D(writeData),
    .clk(clk),
    .reset(control_reset),
    .load(control_reg_load),
    
    .Q(control_out)
);

endmodule
