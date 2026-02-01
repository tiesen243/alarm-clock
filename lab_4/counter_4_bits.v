module counter_4_bits(
	input iClk,
	input iReset_n,
	input iEnable,
	input iClear,
	input iUp_down,
	input [3:0] iInitialValue,
	output reg [3:0] oData
);
	always @(posedge iClk, negedge iReset_n) begin
		if(~iReset_n) begin
			oData <= 3'd0;
		end else if(iClear) begin
			oData <= iInitialValue;
		end else if(iEnable & iUp_down) begin
			oData <= oData + 1'b1;
		end else if(iEnable & ~iUp_down) begin
			oData <= oData - 1'b1;
		end
	end
endmodule
