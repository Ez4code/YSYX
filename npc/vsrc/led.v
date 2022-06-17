module led(
	input clk,
	input rst,
	input a,
	input b,
	output led0
);

	reg ledr;

	always @(posedge clk) begin
		if(rst) begin ledr <= 0; end
		else begin
			ledr <= a ^ b;
		end
	end

	assign led0  = ledr;
endmodule
