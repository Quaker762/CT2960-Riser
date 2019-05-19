module Reset_Sequence
#(parameter BASE_ADDRESS = 16'h0000)
(
    input           sys_clock,
    input           bus_clock,
    input   [15:0]  data_in,
    input           enable,
    input           reset,
    
    output  [15:0]  data_out,
    output  [15:0]  address,
    output          data_dir,
    output          accepted
    
);
localparam reset_write_1     = 4'b0000;
localparam reset_wait        = 4'b0001;
localparam reset_write_0     = 4'b0010;
localparam reset_read_xxxE   = 4'b0011;
localparam reset_read_xxxA   = 4'b0100;   
localparam halt              = 4'b0101;

localparam reset_port                = BASE_ADDRESS + 16'h0006;
localparam read_buffer_status_port   = BASE_ADDRESS + 16'h000E;
localparam read_port                 = BASE_ADDRESS + 16'h000A;

reg [3:0]   current_state;
reg [3:0]   next_state;

reg        count_3u_start;
reg        count_3u_reset_n;
reg        count_3u_done;
wire [15:0] count_out;

reg  [15:0] data_out_r;
reg  [15:0] address_r;
reg         data_dir_r;
reg         accepted_r;

Counter #(16) count_3u
(
    .reset_n    (count_3u_reset_n),
    .count      (count_3u_start),
    .load_n     (1'b1),
    .D          (),
    .clk        (sys_clock),
    
    .Q          (count_out),
    .carry_n    ()
);

always @(count_out)
begin
    if(count_out >= 16'h0096)      // this is 3 micro seconds
        count_3u_done = 1'b1;
    else
        count_3u_done = 1'b0;
end

always @(posedge(sys_clock))
begin  
    if(reset == 1'b1)
        current_state <= reset_write_1;
    else if(bus_clock == 1'b1)    
        current_state <= next_state;
    else
        current_state <= current_state;
end

always @(current_state, count_3u_done, data_in)
begin
    case(current_state)
        halt:
        begin
            next_state <= current_state;
        end
        
        reset_write_1:
        begin
            next_state <= reset_wait;
        end
        
        reset_wait:
        begin
            if(count_3u_done == 1'b1)
                next_state <= reset_write_0;
            else
                next_state <= current_state;
        end
        
        reset_write_0:
        begin
            next_state <= reset_read_xxxE;
        end
        
        reset_read_xxxE:
        begin
            if(data_in[7] == 1'b1)
                next_state <= reset_read_xxxA;
            else
                next_state <= current_state;
        end
        
        reset_read_xxxA:
        begin
            if(data_in == 16'h00AA)
                next_state <= halt;
            else
                next_state <= current_state;
        end
        
        default:
        begin
            next_state <= halt;
        end        
    endcase
end

always @(current_state)
begin
    count_3u_start  <= 1'b0;   
    count_3u_reset_n  <= 1'b0;    
    data_out_r        <= 16'h0000;
    address_r         <= 16'h0000;
    data_dir_r        <= 1'b1;   
    accepted_r        <= 1'b0;
    
    case(current_state)
        halt:
        begin
            address_r         <= reset_port;
            accepted_r        <= 1'b1;
        end
        
        reset_write_1:
        begin
            data_out_r        <= 16'h0001;
            address_r         <= reset_port;
            data_dir_r        <= 1'b1;   
        end
        
        reset_wait:
        begin
            count_3u_reset_n  <= 1'b1;
            count_3u_start    <= 1'b1;
            data_out_r        <= 16'h0001;
            address_r         <= reset_port;  
            data_dir_r        <= 1'b1;               
        end
        
        reset_write_0:
        begin
            data_out_r        <= 16'h0000;
            address_r         <= reset_port;
            data_dir_r        <= 1'b1;      
        end
        
        reset_read_xxxE:
        begin
            address_r         <= read_buffer_status_port;
            data_dir_r        <= 1'b0;
        end
        
        reset_read_xxxA:
        begin
            address_r         <= read_port;
            data_dir_r        <= 1'b0;
        end
     
        default:
        begin
        end
    endcase
end

assign data_out         = enable ? data_out_r       : 16'bzzzzzzzzzzzzzzzz;
assign address          = enable ? address_r        : 16'bzzzzzzzzzzzzzzzz;
assign data_dir         = enable ? data_dir_r       : 1'bz;
assign accepted         = enable ? accepted_r       : 1'bz;

endmodule
