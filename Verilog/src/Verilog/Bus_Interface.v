module Bus_Interface
(
	input data_HPS,
	input address_HPS,
	input data_load,
	input address_load,
	input IOW,
	input IOR,
	input reset,
	input clk,

	inout  [15:0] data_bus,
	output [15:0] address_bus
);

reg [15:0] data_out;
wire [15:0] data_in;

Regiset #(32) address_buffer
(
	.D(address_HPS),
	.clk(clk),
	.reset(reset),
	.load(address_load),
	
	.Q(address_bus)
);

Register #(32) data_buffer
(
	.D(data_in),
	.clk(clk),
	.reset(reset),
	.load(data_load),
	
	.Q(data_out)
);

assign data_bus = ~IOW ? data_out : 16'bzzzzzzzzzzzzzzzz;
assign data_in  = ~IOR ? data_bus : data_HPS;

endmodule
