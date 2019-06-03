module State_Machine
(
    input       [7:0] control_in,
    input             clk,
    input             reset,
    
    output reg        data_load,
    output reg        data_read,
    output reg        address_load,
    output reg        iow,
    output reg        ior,
    output reg        control_reset,
	 output reg 		 data_out,

    output  [3:0]  state_debug
);

localparam BUS_IDLE         = 4'b0000;
localparam BUS_ADDRESS_LOAD = 4'b0001;
localparam BUS_WRITE1       = 4'b0011;
localparam BUS_WRITE2       = 4'b0100;
localparam BUS_WRITE3       = 4'b0101;
localparam BUS_WRITE4       = 4'b0110;
localparam BUS_WRITE5       = 4'b0111;
localparam BUS_WRITE6		 = 4'b1000;
localparam BUS_READ1        = 4'b1001;
localparam BUS_READ2        = 4'b1010;
localparam BUS_READ3        = 4'b1011;
localparam BUS_READ4        = 4'b1100;
localparam BUS_READ5        = 4'b1101;
localparam CONTROL_RESET    = 4'b1110;

reg [3:0] current_state;
reg [3:0] next_state;

always @(posedge(clk), negedge(reset))
begin    
    if(reset == 1'b0)
    begin
        current_state <= BUS_IDLE;
    end
    else 
    begin
        current_state <= next_state;
    end
end

always @(current_state, control_in)
begin
    case(current_state)
        BUS_IDLE:
        begin
            if(control_in[0] == 1'b1 || control_in[1] == 1'b1)
            begin
                next_state <= BUS_ADDRESS_LOAD;
            end
            else
            begin
                next_state <= BUS_IDLE;
            end
        end
        
        BUS_ADDRESS_LOAD:
        begin
            if(control_in[0] == 1'b1)
            begin
                next_state <= BUS_READ1;
            end
            else if(control_in[1] == 1'b1)
            begin
                next_state <= BUS_WRITE1;
            end
            else
            begin
                next_state <= BUS_IDLE;
            end
        end
        
        BUS_WRITE1:
        begin
            next_state <= BUS_WRITE2;
        end
        
        BUS_WRITE2:
        begin
            next_state <= BUS_WRITE3;
        end
        
        BUS_WRITE3:
        begin
            next_state <= BUS_WRITE4;
        end
        
        BUS_WRITE4:
        begin
            next_state <= BUS_WRITE5;
        end
        
        BUS_WRITE5:
        begin
            next_state <= BUS_WRITE6;
        end
		  
		  BUS_WRITE6:
		  begin
            next_state <= CONTROL_RESET;
        end
        
        BUS_READ1:
        begin
            next_state <= BUS_READ2;
            
        end
        BUS_READ2:
        begin
            next_state <= BUS_READ3;
        end
        
        BUS_READ3:
        begin
            next_state <= BUS_READ4;
        end
        
        BUS_READ4:
        begin
            next_state <= BUS_READ5;
        end
        
        BUS_READ5:
        begin
            next_state <= CONTROL_RESET;
        end
        
        CONTROL_RESET:
        begin
            next_state <= BUS_IDLE;
        end
        
    endcase
end

always @(current_state)
begin
    case(current_state)
        BUS_IDLE:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b1;
            ior             <= 1'b1;
            control_reset   <= 1'b1;
			data_out        <= 1'b1;
        end
        
        BUS_ADDRESS_LOAD:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b0;
            iow             <= 1'b1;
            ior             <= 1'b1;
            control_reset   <= 1'b1; 
			data_out        <= 1'b1;			
        end
        
        BUS_WRITE1:
        begin
            data_load       <= 1'b0;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b1;
            ior             <= 1'b1;
            control_reset   <= 1'b1;
			data_out        <= 1'b1;				
        end
        
        BUS_WRITE2:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b1;
            ior             <= 1'b1;
            control_reset   <= 1'b1; 
			data_out        <= 1'b0;
        end
        
        BUS_WRITE3:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b0;
            ior             <= 1'b1;
            control_reset   <= 1'b1;
			data_out        <= 1'b0;
        end
        
        BUS_WRITE4:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b0;
            ior             <= 1'b1;
            control_reset   <= 1'b1;
			data_out        <= 1'b0;
        end
        
        BUS_WRITE5:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b0;
            ior             <= 1'b1;
            control_reset   <= 1'b1;
			data_out        <= 1'b0;	
        end
		  
		  BUS_WRITE6:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b0;
            ior             <= 1'b1;
            control_reset   <= 1'b1;
			data_out        <= 1'b0;	
        end
        
        BUS_READ1:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b1;
            ior             <= 1'b1;
            control_reset   <= 1'b1;
			data_out        <= 1'b1;				
        end
        BUS_READ2:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b1;
            ior             <= 1'b0;
            control_reset   <= 1'b1;
			data_out        <= 1'b1;
        end
        
        BUS_READ3:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b1;
            ior             <= 1'b0;
            control_reset   <= 1'b1;
			data_out        <= 1'b1;
        end
        
        BUS_READ4:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b1;
            ior             <= 1'b0;
            control_reset   <= 1'b1; 
			data_out        <= 1'b1; 				
        end
        
        BUS_READ5:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b0;
            address_load    <= 1'b1;
            iow             <= 1'b1;
            ior             <= 1'b0;
            control_reset   <= 1'b1;
			data_out        <= 1'b1;
        end
        
        CONTROL_RESET:
        begin
            data_load       <= 1'b1;
            data_read       <= 1'b1;
            address_load    <= 1'b1;
            iow             <= 1'b1;
            ior             <= 1'b1;
            control_reset   <= 1'b0;
			data_out        <= 1'b1;
        end
    endcase
end

assign state_debug = current_state;

endmodule
