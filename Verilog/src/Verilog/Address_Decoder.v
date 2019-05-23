/**
 *  Address decoder module. 
 *
 *  Decodes a 3-bit input address from QSYS into a 4-bit output. Each bit a select
 *  pin for a register loacated in a register file. 
 */
module Address_Decoder
(
    input                   ce,     // Chip enable register
    input [2:0]         address, // 4-bit Input address
    output reg [3:0]    out     // Output of the decoder
);


always @(ce)
begin
    if(ce == 1'b1) // The chip is enabled
    begin
        case(address)
        2'b00:
            out <= 4'h01;   // Control register load enable (from HPS)
        2'b01:
            out <= 4'h02;   // Address register load enable (from HPS)
        2'b10:
            out <= 4'h04;   // Data register load enable (from HPS)
        2'b11:
            out <= 4'h08;   // Data Register load enable (from Card)
        default:
            out <= 4'h00;
        endcase
    end
    else
    begin
        out <= 4'h00; // Default value, nothing is selected
    end
end

endmodule 