//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
`timescale 1 ns / 100 ps

module system_tb;

    //----------------------------------------------------------------------------
    // Parameter (may differ for physical synthesis)
    //----------------------------------------------------------------------------
    parameter   tck     = 20;       // clock period in ns
    //----------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------
    /* System signals */
    parameter   CLK_FREQ = 96000000;
    parameter   NUM_PERIP = 8;
    reg         clk     = 0;
    reg         rst_i   = 0;
    
    wire        led;

    /* Control signals */
    reg         ncs_i   = 0;
    reg         nwe_i   = 0;
    reg         nrd_i   = 0;

    reg[13:0]   address_i  = 0;

    wire[7:0]   data_bus;
    
    //----------------------------------------------------------------------------
    // Device Under Test 
    //----------------------------------------------------------------------------
    system #(
        .CLK_FREQ(CLK_FREQ),
        .NUM_PERIP(NUM_PERIP)
    ) system(
        .clk(clk),
        .reset(rst_i),
        .ncs(ncs_i),
        .nwe(nwe_i),
        .nrd(nrd_i),
        .address(address_i),
        .data_bus(data_bus),
        .led(led)
    );
    
    /* Initial values */
    initial begin        
        clk <= 0;
    end

    always begin
        #(tck/2) clk <= ~clk;
    end

    /* Simulation setup */
    initial begin
        $dumpfile("system_tb.vcd");
        $dumpvars(-1, system);
        $dumpvars(-1, encoder0);
        $dumpvars(-1, motor0);
        rst_i <= #80 ~rst_i;
                
        
        
        #(tck*25000*2) $finish;
    end
endmodule
