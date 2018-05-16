`timescale 1ns / 1ps
module reset(clk, sram_data, addr, nwe, nrd, ncs, led, reset);
  input            clk, reset, nwe, ncs, nrd;
  reg           led;
  input [12:0]      addr;
  inout [7:0]      sram_data;

  // synchronize signals                               
  reg    sncs, snwe;
  reg    [12:0] buffer_addr;
  reg    [7:0] buffer_data;  

  // bram interfaz signals
  reg    we;
  reg    w_st;

  reg    [7:0] wdBus;
  wire   [7:0] rdBus;

  //--------------------------------------------------------------------------
  // synchronize assignment
  always  @(negedge clk)
  begin
    sncs   <= ncs;
    snwe   <= nwe;
    buffer_data <= sram_data;
    buffer_addr <= addr;
  end


  //--------------------------------------------------------------------------
  // Blinking LED
  reg [24:0]  counter;
  always @(posedge clk) begin
    if(~reset) 
      counter <= 0;
    else 
      counter <= counter + 1;
  end 
  assign led = counter[24];

endmodule

