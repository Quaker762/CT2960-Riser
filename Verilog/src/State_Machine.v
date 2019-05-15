module State_Machine
(
    input               sys_clock,
    input       [15:0]  data_in,
    input               count_3u_finish,
    input               set_safe,
    
    output reg          count_3u_start,
    output reg          count_3u_reset,
    output reg  [15:0]  data_out,
    output reg  [15:0]  address,
    output reg          data_dir
    
);
parameter safe              = 4'b0000;
parameter reset_write_1     = 4'b0001;
parameter reset_wait        = 4'b0010;
parameter reset_write_0     = 4'b0011;
parameter reset_read_2xE    = 4'b0100;
parameter reset_read_2xA    = 4'b0101;   

parameter reset_port                = 16'h0206;
parameter read_buffer_status_port   = 16'h020E;
parameter read_port                 = 16'h020A;

reg [3:0] current_state;
reg [3:0] next_state;

always @(posedge(sys_clock), posedge(set_safe))
begin
    if(set_safe == 1'b1)
        current_state <= safe;
    else    
        current_state <= next_state;
end

always @(current_state, count_3u_finish, data_in)
begin
    case(current_state)
        safe:
        begin
            next_state <= reset_write_1;
        end
        
        reset_write_1:
        begin
            next_state <= reset_wait;
        end
        
        reset_wait:
        begin
            if(count_3u_finish == 1'b1)
                next_state <= reset_write_0;
            else
                next_state <= current_state;
        end
        
        reset_write_0:
        begin
            next_state <= reset_read_2xE;
        end
        
        reset_read_2xE:
        begin
            if(data_in[7] == 1'b1)
                next_state <= reset_read_2xA;
            else
                next_state <= current_state;
        end
        
        reset_read_2xA:
        begin
            if(data_in == 16'hAA)
                next_state <= safe;
            else
                next_state <= current_state;
        end
        
        default:
        begin
            next_state <= safe;
        end        
    endcase
end

always @(current_state)
begin
    count_3u_start  <= 1'b0;   
    count_3u_reset  <= 1'b1;    
    data_out        <= 16'h0000;
    address         <= 16'h0000;
    data_dir        <= 1'b1;   
    
    case(current_state)
        safe:
        begin
        end
        
        reset_write_1:
        begin
            data_out        <= 16'h0001;
            address         <= reset_port;
            data_dir        <= 1'b1;   
        end
        
        reset_wait:
        begin
            count_3u_reset  <= 1'b0;
            count_3u_start  <= 1'b1;  
        end
        
        reset_write_0:
        begin
            data_out        <= 16'h0000;
            address         <= reset_port;
            data_dir        <= 1'b1;      
        end
        
        reset_read_2xE:
        begin
            address         <= read_buffer_status_port;
            data_dir        <= 1'b0;
        end
        
        reset_read_2xA:
        begin
            address         <= read_port;
            data_dir        <= 1'b0;
        end
     
        default:
        begin
        end
    endcase
end

endmodule
