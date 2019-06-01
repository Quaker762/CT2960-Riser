/**
 *  2:1 Multiplexer. 
 *
 *  Given 2 32-bit inputs, select only a single 32-bit value
 *  to be asserted as output.
 */
module Mux_3_To_1 #(parameter WIDTH = 32)
(
    input   [2:0]               address,        // Three-bit register address from QSYS
    
    // Register inputs
    input [WIDTH-1:0]           reg1Data,
    input [WIDTH-1:0]           reg2Data,
    input [WIDTH-1:0]           reg3Data,
    
    input                           en,         // Enable control signal
    
    output reg [WIDTH-1:0]  out             // Output data register (the selected register)
);

always @(en, address, reg1Data, reg2Data, reg3Data)
begin
    if(en == 1'b1) // Should we assert the output to the bus?
    begin
        case(address)
        3'b001:
            out <= reg1Data;
        3'b100:
            out <= reg2Data;
        3'b101:
            out <= reg3Data;
        default:
            out <= 32'hzzzzzzzz;
        endcase
    end
    else
    begin
        out <= 32'hzzzzzzzz; // Tri-state. High impedance output if en is low
    end
end


endmodule 