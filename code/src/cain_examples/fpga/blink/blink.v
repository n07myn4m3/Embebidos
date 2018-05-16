`timescale 1ns / 1ps
module blink(clk, led);
  input            clk;
  output           led;

  reg [24:0]  counter;
  always @(posedge clk) begin
    counter <= counter + 1;
  end 
  assign led = counter[24];

endmodule

