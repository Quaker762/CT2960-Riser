module Register_File
(
    input           write,
    input           read,
    input   [31:0]  writedata,
    output  [31:0]  readdata,
    input   [2:0]   address,
    input           clk,
    
    input   [15:0]  data_bus_in,
    
    input           reset,
    input           control_reset,
    
    
    output  [15:0]  data_out_HPS,
    output  [15:0]  address_out_HPS,
    output  [7:0]   control_out
);

wire [5:0] decodedAddr;

wire [31:0] data_out;
wire [31:0] test_out;

wire control_reg_load_w	= decodedAddr[0];
wire control_reg_load_r	= decodedAddr[1];
wire addr_reg_load 		= decodedAddr[2];
wire data_reg_load_w	= decodedAddr[3];
wire data_reg_load_r    = decodedAddr[4];
wire test               = decodedAddr[5];

Address_Decoder decoder
(
	.ce(write || read),
	.address(address),
	.out(decodedAddr)
);

Mux_3_To_1 output_mux
(
    .en(read),  
    .address(address),
    .reg1Data(control_out),
    .reg2Data(data_out), 
    .reg3Data(test_out),
    .out(readdata)
);

Register #(32) address_reg
(
    .D(writedata),
    .clk(clk),
    .reset(reset),
    .load(~addr_reg_load),
    
    .Q(address_out_HPS)
);

// This needs to be a 2 input Flip flop!
RegisterRW #(32) data_reg
(
    .D(writedata),
    .D2(data_bus_in),
    .clk(clk),
    .reset(reset),
    .load(~data_reg_load_w),
    .load2(~data_reg_load_r),
    
    .Q(data_out)
);

RegisterRW #(32) control_reg
(
    .D(writedata),
    .D2(control_out),
    .clk(clk),
    .reset(reset && control_reset),
    .load(~control_reg_load_w),
    .load2(~control_reg_load_r),
    
    .Q(control_out)
);

Register #(32) test_reg
(
    .D(32'ha5a5a5a5),
    .clk(clk),
    .reset(reset),
    .load(~test),
    
    .Q(test_out)
);

assign data_out_HPS = data_out;

endmodule
