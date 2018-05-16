`timescale 1ns / 1ps
module system #(
    parameter           CLK_FREQ        = 96000000,
    parameter           NUM_PERIP       = 8
)(
    clk,
    reset,
    ncs,
    nwe,
    nrd,
    address,
    data_bus,
    led
);

  input               clk;
  input               reset;

  input               ncs;
  input               nwe;
  input               nrd;

  input[13:0]         address;
  inout[7:0]          data_bus;

  // Peripherals in/out singnals.
  output              led;

  // Internal signals
  wire                rst;

  // Synchronization signals
  reg                 sncs            = 0;
  reg                 snwe            = 0;
  reg[13:0]           buffer_address  = 0;
  reg[7:0]            buffer_data     = 0;

  // Peripheral control signals
  reg[7:0]            rd_bus          = 0;
  reg                 we              = 0;

  // Internal registers
  reg                 w_st            = 0;
  reg[3:0]            replay          = 0;

  assign rst = ~reset;

  // Ensurnig tenth address pin to GND (It's actualy no rounted in PCB).
  assign address[10] = 0;

  // Bidirectinal bus control signal
  assign data_bus = (nrd | ncs) ? 8'bZ : rd_bus;

  // LED
  //assign led = 1;

  // Synchronization logic block
  always @(negedge clk) begin
      sncs <= ncs;
      snwe <= nwe;
      buffer_data <= data_bus;
      buffer_address <= address;
  end

  // Write pulse generator block (write enable)
  always @(posedge clk) begin
      if(rst) begin
          w_st <= 0;
          we <= 0;
      end else begin
          case(w_st)
              1'b0: begin
                  we <= 0;
                  if(sncs | snwe)
                      w_st <= 1;
              end
              1'b1: begin
                  if(~(sncs | snwe)) begin
                      we <= 1;
                      w_st <= 0;
                  end else
                      we <= 0;
              end
              default: begin
                  we <= 0;
                  w_st <= 0;
              end
          endcase
      end
  end
  
  // Blink
  reg [24:0]  counter;
  always @(posedge clk) begin
    counter <= counter + 1;
  end 
  assign led = counter[24];
endmodule
