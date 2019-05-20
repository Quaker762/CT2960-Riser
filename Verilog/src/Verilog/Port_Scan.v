module Port_Scan
(
    input           sys_clock,
    input           bus_clock,
    input   [15:0]  data_in,
    input           reset,
    
    output  [15:0]  data_out,
    output  [15:0]  address,
    output          data_dir
    
);

reg [4:0]   current_state;
reg [4:0]   next_state;

reg [15:0]   enable;
wire         accepted;

genvar i;
generate
    for(i = 4'h0; i < 5'h10; i = i + 16'h1) begin: gen
        localparam [15:0] j = 16'h0200 + (i << 4);
        Reset_Sequence #(j) reset
        (
            .sys_clock(sys_clock),
            .bus_clock(bus_clock),
            .data_in(data_in),
            .enable(enable[i]),
            .reset(~enable[i]),
                
            .data_out(data_out),
            .address(address),
            .data_dir(data_dir),
            .accepted(accepted)
        );
    end
endgenerate

wire carry_n;

Counter #(26) count_1s
//Counter #(8) count_1s
(
    .reset_n    (!reset),
    .count      (1'b1),
    .load_n     (1'b1),
    .D          (),
    .clk        (sys_clock),
    
    .Q          (),
    .carry_n    (carry_n)
);

always @(posedge(sys_clock))
begin  
    if(reset == 1'b1)
        current_state <= 5'h10;
    else if(accepted == 1'b1)
        current_state <= current_state;
    else if(carry_n == 1'b0)    
        current_state <= next_state;
    else
        current_state <= current_state;
end

always @(current_state)
begin
    case(current_state)
        5'h00:
        begin
            next_state <= 5'h01;
        end       
        5'h01:
        begin
            next_state <= 5'h02;
        end
        5'h02:
        begin
            next_state <= 5'h03;
        end        
        5'h03:
        begin
            next_state <= 5'h04;
        end       
        5'h04:
        begin
            next_state <= 5'h05;
        end       
        5'h05:
        begin
            next_state <= 5'h06;
        end       
        5'h06:
        begin
            next_state <= 5'h07;
        end        
        5'h07:
        begin
            next_state <= 5'h08;
        end   
        5'h08:
        begin
            next_state <= 5'h09;
        end   
        5'h09:
        begin
            next_state <= 5'h0A;
        end   
        5'h0A:
        begin
            next_state <= 5'h0B;
        end   
        5'h0B:
        begin
            next_state <= 5'h0C;
        end   
        5'h0C:
        begin
            next_state <= 5'h0D;
        end   
        5'h0D:
        begin
            next_state <= 5'h0E;
        end   
        5'h0E:
        begin
            next_state <= 5'h0F;
        end   
        5'h0F:
        begin
            next_state <= 5'h00;
        end
        5'h10:
        begin
            next_state <= 5'h00;
        end          
        default:
        begin
            next_state <= 5'h00;
        end           
    endcase
end

always @(current_state)
begin
    case(current_state)
        5'h00:
        begin
            enable <= 16'b0000000000000001;
        end       
        5'h01:
        begin
            enable <= 16'b0000000000000010;
        end
        5'h02:
        begin
            enable <= 16'b0000000000000100;
        end        
        5'h03:
        begin
            enable <= 16'b0000000000001000;
        end       
        5'h04:
        begin
            enable <= 16'b0000000000010000;
        end       
        5'h05:
        begin
            enable <= 16'b0000000000100000;
        end       
        5'h06:
        begin
            enable <= 16'b0000000001000000;
        end        
        5'h07:
        begin
            enable <= 16'b0000000010000000;
        end   
        5'h08:
        begin
            enable <= 16'b0000000100000000;
        end   
        5'h09:
        begin
            enable <= 16'b0000001000000000;
        end   
        5'h0A:
        begin
            enable <= 16'b0000010000000000;
        end   
        5'h0B:
        begin
            enable <= 16'b0000100000000000;
        end   
        5'h0C:
        begin
            enable <= 16'b0001000000000000;
        end   
        5'h0D:
        begin
            enable <= 16'b0010000000000000;
        end   
        5'h0E:
        begin
            enable <= 16'b0100000000000000;
        end   
        5'h0F:
        begin
            enable <= 16'b1000000000000000;
        end
        5'h10:
        begin
            enable <= 16'b0000000000000000;
        end         
        default:
        begin
            enable <= 16'b0000000000000000;
        end           
    endcase
end

endmodule
