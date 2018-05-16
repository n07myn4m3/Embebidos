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
  
  // Chip Select signal
  wire[7:0]           chip_select;

  // Peripheral data out wires
  wire[7:0]           data_out_p1;
  wire[7:0]           data_out_p2;
  wire[7:0]           data_out_p3;
  wire[7:0]           data_out_p4;
  wire[7:0]           data_out_p5;
  wire[7:0]           data_out_p6;
  wire[7:0]           data_out_p7;
  wire[7:0]           data_out_p8;

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
  
  // Data out handler
  always @(posedge clk) begin
    if(rst)
      rd_bus <= 0;
    else begin
      case(chip_select)
      8'b00000001: rd_bus <= data_out_p1;
      8'b00000010: rd_bus <= data_out_p2;
      8'b00000100: rd_bus <= data_out_p3;
      8'b00001000: rd_bus <= data_out_p4;
      8'b00010000: rd_bus <= data_out_p5;
      8'b00100000: rd_bus <= data_out_p6;
      8'b01000000: rd_bus <= data_out_p7;
      8'b10000000: rd_bus <= data_out_p8;
      default: rd_bus <= 0;
      endcase
    end
  end
  
  // Blink
  reg [24:0]  counter;
  always @(posedge clk) begin
    counter <= counter + 1;
  end 
  assign led = counter[24];
  
  chip_select chip_select0(
    .clk(clk), 
    .rst(rst), 
    .buffer_address({buffer_address[13:11], buffer_address[9:8]}),
    .chip_select(chip_select)
  );
  
  p_example p_example0(
    .clk(clk),
    .rst(rst),
    .cs(chip_select[0]),
    .we(we), 
    .buffer_data(buffer_data),
    .buffer_address(buffer_address[7:0]), 
    .data_out(data_out_p1)
  );
  
endmodule
