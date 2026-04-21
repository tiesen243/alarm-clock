module clock(
  input CLOCK_50, UART_RXD,
  input [3:0] KEY,
  input [6:0] SW,

  inout [8:0] GPIO,

  output UART_TXD, LCD_ON, LCD_BLON, LCD_EN, LCD_RW, LCD_RS,
  output [7:0] LCD_DATA,
  output [6:0] HEX0, HEX1,
  output [0:0] LEDR
);
  system u0 (
    .clk_clk                                   (CLOCK_50),       //                                   clk.clk

    .uart_0_external_connection_rxd            (UART_RXD),       //            uart_0_external_connection.rxd
    .uart_0_external_connection_txd            (UART_TXD),       //                                      .txd

    .button_external_connection_export         (GPIO[3:0]),      //         button_external_connection.export
    .switch_external_connection_export         (SW[6:0]),        //         switch_external_connection.export

    .lcd_on_external_connection_export         (LCD_ON),         //         lcd_on_external_connection.export
    .lcd_blon_external_connection_export       (LCD_BLON),       //       lcd_blon_external_connection.export
    .lcd_en_external_connection_export         (LCD_EN),         //         lcd_en_external_connection.export
    .lcd_rw_external_connection_export         (LCD_RW),         //         lcd_rw_external_connection.export
    .lcd_rs_external_connection_export         (LCD_RS),         //         lcd_rs_external_connection.export
    .lcd_data_external_connection_export       (LCD_DATA),       //       lcd_data_external_connection.export

    .hex_0_external_connection_export          (HEX0),           //          hex_0_external_connection.export
    .hex_1_external_connection_export          (HEX1),           //          hex_1_external_connection.export

    .buzzer_external_connection_export         (GPIO[8]),        //         buzzer_external_connection.export

    .matrix_led_din_external_connection_export (GPIO[4]),        // matrix_led_din_external_connection.export
    .matrix_led_cs_external_connection_export  (GPIO[5]),        //  matrix_led_cs_external_connection.export
    .matrix_led_clk_external_connection_export (GPIO[6])         // matrix_led_clk_external_connection.export
  );
endmodule
