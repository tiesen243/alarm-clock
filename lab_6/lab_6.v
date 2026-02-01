module lab_6(
	input  CLOCK_50,
	input  [1:0] KEY,
	output LCD_ON, LCD_BLON, LCD_EN, LCD_RW, LCD_RS,
	output [7:0] LCD_DATA
);
	system u0 (
		.clk_clk                             (CLOCK_50),
		.reset_reset_n                       (KEY[0]),
		.lcd_on_external_connection_export   (LCD_ON),
		.lcd_blon_external_connection_export (LCD_BLON),
		.lcd_en_external_connection_export   (LCD_EN),
		.lcd_rw_external_connection_export   (LCD_RW),
		.lcd_rs_external_connection_export   (LCD_RS),
		.lcd_d_external_connection_export    (LCD_DATA),
		.button_external_connection_export   (KEY[1])
	);
endmodule
