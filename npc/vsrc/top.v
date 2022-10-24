module top (
	input clk,
	input rst,
	output [15:0] led0
);

light led1(
	.clk(clk),
	.rst(rst),
	.led(led0)
);
endmodule

