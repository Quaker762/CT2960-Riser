module Bus_Interface
(
    input  [15:0]   data_HPS_in,
    input  [15:0]   data_bus_in,
    input  [15:0]   address_HPS_in,
    input           data_load,
    input           address_load,
    input           IOW,
    input           IOR,
    input           reset,
    input           clk,

    output [15:0]   data_bus_out,
    output [15:0]   data_HPS_out,
    output [15:0]   address_bus
);

wire [15:0] data_out;
wire [15:0] data_in;

Register #(32) address_buffer
(
    .D(address_HPS_in),
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

assign data_HPS_out = data_out;
assign data_bus_out = ~IOW ? data_out       : 16'bzzzzzzzzzzzzzzzz;
assign data_in      = ~IOR ? data_bus_in    : data_HPS_in;

endmodule
