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
    
    input           SW0,
    output          LED0,
    
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
wire [3:0]  dack;
wire        aen = 1'b0;
 

/* Control the bi-directional data pins */
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
//assign IOW      
//assign IOR      

assign DACK1    = dack[0];
assign DACK3    = dack[1];
assign DACK5    = dack[2];
assign DACK7    = dack[3];

assign AEN      = aen;

endmodule
