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
    
    output          RESET,
    output          IOW,
    output          IOR,
    output          DACK1,
    output          DACK3,
    output          DACK5,
    output          DACK7,
    output          AEN
    
);
wire        clk_5MHz;

wire [15:0] data_out;    
wire [15:0] data_in;
wire [15:0] address;

wire [3:0]  irq;
wire [3:0]  drq;

wire        reset;
wire        data_dir;        // direction of data bus
wire [3:0]  dack;
wire        aen;

wire pll_reset = 1'b0;
wire count_3u_finish;  
wire count_3u_start;  
wire count_3u_reset; 
wire set_safe; 

/*
PLL5MHz sys_clock
(
    .refclk     (clk_50MHz),
    .rst        (pll_reset),
    .outclk_0   (clk_5MHz)
);
*/

Counter #(4) count_3u
(
    .reset_n    (!count_3u_reset),
    .count      (count_3u_start),
    .load_n     (1'b1),
    .D          (),
    .clk        (clk_5MHz),
    
    .Q          (),
    .carry_n    (count_3u_finish)
);

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
assign IOW      = data_dir;
assign IOR      = ~data_dir;

assign DACK1    = dack[0];
assign DACK3    = dack[1];
assign DACK5    = dack[2];
assign DACK7    = dack[3];

assign AEN      = aen;

endmodule
