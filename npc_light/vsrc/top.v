module top (
	input clk,
	input rst,
	output reg [15:0] led
);

light light1(
	.clk(clk),
	.rst(rst),
	.led(led)
);
endmodule

