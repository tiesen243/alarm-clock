module fpga(
    input  CLOCK_50, UART_RXD,
	 input  [3:0] KEY,
	 input  [6:0] SW,
	 
	 inout  [9:0] GPIO,

	 output UART_TXD, LCD_ON, LCD_BLON, LCD_EN, LCD_RW, LCD_RS,
	 output [7:0] LCD_DATA,
	 output [6:0] HEX0, HEX1, HEX2,
	 output [0:0] LEDR
);
	system u0(
	  .clk_clk                             (CLOCK_50),

	  .uart_0_external_connection_rxd      (UART_RXD),
	  .uart_0_external_connection_txd      (UART_TXD),

	  .lcd_on_external_connection_export   (LCD_ON),
	  .lcd_blon_external_connection_export (LCD_BLON),
	  .lcd_en_external_connection_export   (LCD_EN),
	  .lcd_rw_external_connection_export   (LCD_RW),
	  .lcd_rs_external_connection_export   (LCD_RS),
	  .lcd_d_external_connection_export    (LCD_DATA),

	  .button_external_connection_export   (GPIO[3:0]),
	  .switch_external_connection_export   (SW[6:0]),

     .matrix_led_din_external_connection_export (GPIO[4]),
	  .matrix_led_cs_external_connection_export  (GPIO[5]),
	  .matrix_led_clk_external_connection_export (GPIO[6]),
	  
	  .buzz_external_connection_export     (GPIO[8]),
	  .led_external_connection_export      (GPIO[9]),
	  .hex0_external_connection_export     (HEX0),
     .hex1_external_connection_export     (HEX1),
     .hex2_external_connection_export     (HEX2)
	);
endmodule
