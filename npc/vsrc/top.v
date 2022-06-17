module top (
	input clk,
	input rst,
	input a,
	input b,
	output led0
);

led led1(
	.clk(clk),
	.rst(rst),
	.a(a),
	.b(b),
	.led0(led0)
);
endmodule

