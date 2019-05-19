module CT2960_Riser
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
    
    input           SW0,
    
    output          RESET,
    output          IOW,
    output          IOR,
    output          DACK1,
    output          DACK3,
    output          DACK5,
    output          DACK7,
    output          AEN
    
);

wire [15:0] data_out;    
wire [15:0] data_in;
wire [15:0] address;

wire [3:0]  irq;
wire [3:0]  drq;
wire        reset;
wire        data_dir;        // direction of data bus
wire [3:0]  dack;
wire        aen;

/*
State_Machine state_machine
(
    .sys_clock          (clk_5MHz),
    .data_in            (data_in),
    .count_3u_finish    (count_3u_finish),
    .set_safe           (set_safe),
    
    .count_3u_reset     (count_3u_reset),
    .count_3u_start     (count_3u_start),
    .data_out           (data_out),
    .address            (address),
    .data_dir           (data_dir)
);
*/

reg        bus_clock;
wire [3:0] bus_clock_out;  

Counter #(4) bus_clock_counter
(
    .reset_n    (SW0 || !bus_clock),
    .count      (1'b1),
    .load_n     (1'b1),
    .D          (),
    .clk        (clk_50MHz),
    
    .Q          (bus_clock_out),
    .carry_n    ()
);

always @(bus_clock_out)
begin
    if(bus_clock_out == 4'hC) //4MHz
        bus_clock = 1'b1;
    else
        bus_clock = 1'b0;
end

Port_Scan port_scan
(
    .sys_clock(clk_50MHz),
    .bus_clock(bus_clock),
    .data_in(data_in),
    .reset(!SW0),

    .data_out(data_out),
    .address(address),
    .data_dir(data_dir)
);

/* Control the bi-directional data pins */
assign D        = data_dir ? data_out : 16'bzzzzzzzzzzzzzzzz;
assign data_in  = D;

assign A        = address;

assign irq[0]   = IRQ2;
assign irq[1]   = IRQ5;
assign irq[2]   = IRQ7;
assign irq[3]   = IRQ10;

assign drq[0]   = DRQ1;
assign drq[1]   = DRQ3;
assign drq[2]   = DRQ5;
assign drq[3]   = DRQ7;

assign RESET    = reset;

/* Make sure we only ever read OR write */
assign IOW      = ~data_dir;
assign IOR      = data_dir;

assign DACK1    = dack[0];
assign DACK3    = dack[1];
assign DACK5    = dack[2];
assign DACK7    = dack[3];

assign AEN      = aen;

endmodule
