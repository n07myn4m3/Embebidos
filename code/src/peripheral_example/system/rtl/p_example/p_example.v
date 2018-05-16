`timescale 1ns / 1ps
module p_example(
    clk,
    rst,
    cs,
    we, 
    buffer_data, 
    buffer_address, 
    data_out
);
    
    input           clk;
    input           rst;
    input           cs;
    input           we;
    input[7:0]      buffer_data;
    input[7:0]      buffer_address;
    
    output reg[7:0] data_out;
    
    reg[7:0] data1 = 0;
    reg[7:0] data2 = 0;
    reg[7:0] data3 = 0;
    reg[7:0] data4 = 0;
    
  always @(posedge clk) begin
    if(rst) begin
      data_out <= 8'b0;
      data1 <= 8'h00;
      data2 <= 8'h00;
      data3 <= 8'h00;
      data4 <= 8'h00;
    end else if(cs) begin
      if(we) begin
        case(buffer_address)
        8'h00: data1 <= buffer_data;
        8'h04: data2 <= buffer_data;
        8'h08: data3 <= buffer_data;
        8'h0C: data4 <= buffer_data;
        8'h10: begin
          data1 <= 8'h00;
          data2 <= 8'h00;
          data3 <= 8'h00;
          data4 <= 8'h00;
        end
        default: data_out <= data_out;
        endcase
      end else begin
        case(buffer_address)
          8'h00: data_out <= data1;
          8'h04: data_out <= data2;
          8'h08: data_out <= data3;
          8'h0C: data_out <= data4;
          default: data_out <= 8'h00;
        endcase
      end
    end else begin
        data_out <= 8'h00;
      end
  end
endmodule
