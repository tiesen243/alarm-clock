module counter_4_bits_tb();
	reg clk;
	reg reset_n;
	reg enable;
	reg clear;
	reg up_down;
	reg [3:0] initial_value;
	wire [3:0] count_value;
	
	// ==============================================
	// Initialize clk signal
	// ==============================================
	parameter PERIOD = 10; // 50 MHz clock
	parameter PERIODx20 = (20 * PERIOD);
	parameter PERIODx40 = (40 * PERIOD);
	parameter PERIODx100 = (100 * PERIOD);
	
	initial begin
		clk <= 1'b0;
		forever # (PERIOD) clk <= ~clk;
	end
	
	// ==============================================
	// Initialize reset_n signal
	// ==============================================
	initial begin
		reset_n <= 1'b0;
		#(PERIODx20)// reset_n is asserted in 10 clocks
		@(negedge clk) reset_n <= 1'b1;
	end
	
	// ==============================================
	// Initialize clear, enable, up_down signal
	// ==============================================
	initial begin
		clear <= 1'b0;
		enable <= 1'b0;
		up_down <= 1'b0;
		#(PERIODx40) clear <= 1'b1;
		@(negedge clk) clear <= 1'b0;
		up_down <= 1'b1;
		#(PERIODx40) enable <= 1'b1;
		#(PERIODx100) up_down <= 1'b0;
		#(PERIODx100) enable <= 1'b0;
	end
	
	// ===============================================
	// Initialize initial_value
	// ===============================================
	initial begin
		initial_value <= 4'h3;
	end
	
	counter_4_bits COUNTER4BITS(
		// Inputs
		.iClk (clk),
		.iReset_n (reset_n),
		.iEnable (enable),
		.iClear (clear),
		.iUp_down (up_down),
		.iInitialValue (initial_value),
		// Outputs
		.oData(count_value)
	);
endmodule
