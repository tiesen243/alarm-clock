module lab_7(
	input CLOCK_50, UART_RXD,
	input [0:0] KEY,
	output UART_TXD
);
	system u0 (
		.clk_clk                        	  (CLOCK_50),
		.uart_0_external_connection_rxd    (UART_RXD),
		.uart_0_external_connection_txd 	  (UART_TXD),
		.button_external_connection_export (KEY[0])
	);
endmodule
