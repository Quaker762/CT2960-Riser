module Bus_Interface
(
    input  [15:0]   data_HPS_in,
    input  [15:0]   data_bus_in,
    input  [15:0]   address_HPS_in,
    input           data_load,
    input           address_load,
    input           iow,
    input           ior,
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

Register #(16) data_buffer
(
    .D(data_HPS_in),
    .clk(clk),
    .reset(reset),
    .load(data_load),
    
    .Q(data_out)
);

assign IOW = iow;
assign IOR = ior;

assign data_HPS_out = (~IOR && ~data_load)  ? data_bus_in    : 16'bzzzzzzzzzzzzzzzz;
assign data_bus_out = ~IOW                  ? data_out       : 16'bzzzzzzzzzzzzzzzz;

endmodule
