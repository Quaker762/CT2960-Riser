/*
 * Upwards counter with presetable value and carry flag
 *
 * @author      Matthew Atkin/s3603797
 * @version     1.0
 * @since       15 Apr 2019
 */
 module Counter
 #(parameter WIDTH = 4, MAX = ((2 ** WIDTH)-1))
(
    input               reset_n,
    input               count,
    input               load_n,
    input [WIDTH-1:0]   D,
    input               clk,
    
    output [WIDTH-1:0]  Q,
    output              carry_n
);
reg [WIDTH-1:0] counterValue;

always @(posedge(clk)) 
begin
    if(reset_n == 1'b0) 
    begin
        counterValue = 0;
    end else if(load_n == 1'b0) 
    begin
        counterValue = D;
    end else if(count == 1'b1) 
    begin
		if(counterValue == MAX)
		begin
			counterValue = 0;
		end else 
		begin
			counterValue = counterValue + 1'b1;
		end	
    end else 
    begin
        counterValue = counterValue;
    end
end

assign Q = counterValue;
assign carry_n = &counterValue;

endmodule