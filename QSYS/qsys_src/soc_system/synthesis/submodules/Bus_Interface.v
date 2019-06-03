module Bus_Interface
(
    input  [15:0]   data_HPS_in,
    input  [15:0]   data_bus_in,
    input  [15:0]   address_HPS_in,
    input           data_load,
    input           data_read,
    input           address_load,
    input           iow,
    input           ior,
	 input           data_write,
    input           reset,
    input           clk,

    output          IOW,
    output          IOR,
    output [15:0]   data_bus_out,
    output [15:0]   data_HPS_out,
    output [15:0]   address_bus
);

wire [15:0] data_out;

Register #(16) address_buffer
(
    .D(address_HPS_in),
    .clk(clk),
    .reset(reset),
    .load(address_load),
    
    .Q(address_bus)
);

Register #(16) data_out_buffer
(
    .D(data_HPS_in),
    .clk(clk),
    .reset(reset),
    .load(data_load),
    
    .Q(data_out)
);

Register #(16) data_in_buffer
(
    .D(data_bus_in),
    .clk(clk),
    .reset(reset),
    .load(data_read),
    
    .Q(data_HPS_out)
);

assign IOW = iow;
assign IOR = ior;

assign data_bus_out = ~data_write                 ? data_out       : 16'bzzzzzzzzzzzzzzzz;

endmodule
