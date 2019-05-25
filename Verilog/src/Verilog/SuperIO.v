module SuperIO
(
    input           clk_50MHz,
    
    inout   [15:0]  D,
    output  [15:0]  A,
    
    input           IRQ2,
    input           IRQ5,
    input           IRQ7,
    input           IRQ10,
    input           DRQ1,
    input           DRQ3,
    input           DRQ5,
    input           DRQ7,
    
    input           global_reset,
    output          LED0,
    
    output          RESET,
    output          IOW,
    output          IOR,
    output          DACK1,
    output          DACK3,
    output          DACK5,
    output          DACK7,
    output          AEN,
    
    input           write,
    input           read,
    input   [31:0]  writedata,
    output  [31:0]  readdata,
    output          clk,
    input   [2:0]   address
    
);

wire [15:0] data_bus_out;    
wire [15:0] data_bus_in;
wire [15:0] address_bus;

wire [3:0]  irq;
wire [3:0]  drq;
wire        reset;
wire [3:0]  dack;
wire        aen = 1'b0;

wire        clk_bus;

wire data_load;
wire address_load;
wire control_reset;
wire [7:0] control;
wire [15:0] data_HPS_in;
wire [15:0] data_HPS_out;
wire [15:0] address_HPS_in;

wire ior;
wire iow;

wire hps_write;
wire hps_read;
/*
Bus_Clock_8MHz bus_clock_pll
(
    .refclk(clk_50MHz),
    .rst(!global_reset),
    .outclk_0(clk_bus)
);
*/
State_Machine state_machine
(
    .control_in(control),
    .clk(clk_bus),
    .reset(global_reset),
    
    .data_load(data_load),
    .address_load(address_load),
    .iow(iow),
    .ior(ior),
    .control_reset(control_reset)
);

Bus_Interface bus_interface
(
    .data_HPS_in(data_HPS_in),
    .data_bus_in(data_bus_in),
    .address_HPS_in(address_HPS_in),
    .data_load(data_load),
    .address_load(address_load),
    .iow(iow),
    .ior(ior),
    .reset(global_reset),
    .clk(clk_bus),

    .IOW(IOW),
    .IOR(IOR),
    .data_bus_out(data_bus_out),
    .data_HPS_out(data_HPS_out),
    .address_bus(address_bus)
);

Register_File register_file
(
    .write(write),
    .read(read),
    .writedata(writedata),
    .readdata(readdata),
    .address(address),
    .clk(clk_bus),
    .data_bus_in(data_HPS_out),
    .reset(global_reset),
    .control_reset(control_reset),
    .data_out_HPS(data_HPS_in),
    .address_out_HPS(address_HPS_in),
    .control_out(control)
    
);

assign D            = data_bus_out;

assign data_bus_in  = D;

assign A            = address_bus;

assign irq[0]       = IRQ2;
assign irq[1]       = IRQ5;
assign irq[2]       = IRQ7;
assign irq[3]       = IRQ10;

assign drq[0]       = DRQ1;
assign drq[1]       = DRQ3;
assign drq[2]       = DRQ5;
assign drq[3]       = DRQ7;

assign RESET        = reset;   

assign DACK1        = dack[0];
assign DACK3        = dack[1];
assign DACK5        = dack[2];
assign DACK7        = dack[3];

assign AEN          = aen;

assign clk          = clk_bus;

endmodule
