`timescale 1ns / 1ps
module chip_select(clk, rst, buffer_address, chip_select);
    
    input           clk;
    input           rst;
    input[4:0]      buffer_address;
    
    output reg[7:0] chip_select;
    
    always @(posedge clk) begin
        if(rst)
            chip_select <= 8'b00000000;
        else
            case(buffer_address)
            5'b00001: chip_select <= 8'b00000001; //0x63000100
            5'b00010: chip_select <= 8'b00000010; //0x63000200
            5'b00011: chip_select <= 8'b00000100; //0x63000300
            5'b00100: chip_select <= 8'b00001000; //0x63000800
            5'b00101: chip_select <= 8'b00010000; //0x63000900
            5'b00110: chip_select <= 8'b00100000; //0x63000A00
            5'b00111: chip_select <= 8'b01000000; //0x63000B00
            5'b01000: chip_select <= 8'b10000000; //0x63001000
            default: chip_select <= 8'b00000000;
            endcase
    end
endmodule
