/**
 *  4:1 Multiplexer. 
 *
 *  Given 4 32-bit inputs, select only a single 32-bit value
 *  to be asserted as output.
 */
module Mux_4_To_1 #(parameter WIDTH = 32)
(
    input   [2:0]                   address,        // Two-bit register address from QSYS
    
    // Register inputs
    input [WIDTH-1:0]           reg1Data,
    input [WIDTH-1:0]           reg2Data,
    input [WIDTH-1:0]           reg3Data,
    input [WIDTH-1:0]           reg4Data,
    
    input                           en,         // Enable control signal
    
    output reg [WIDTH-1:0]  out             // Output data register (the selected register)
);

always @(en)
begin
    if(en == 1'b1) // Should we assert the output to the bus?
    begin
        case(address)
        2'b00:
            out <= reg1Data;
        2'b01:
            out <= reg1Data;
        2'b10:
            out <= reg1Data;
        2'b11:
            out <= reg1Data;
        default:
            out <= 32'h00000000;
        endcase
    end
    else
    begin
        out <= 32'hzzzzzzzz; // Tri-state. High impedance output if en is low
    end
end


endmodule 