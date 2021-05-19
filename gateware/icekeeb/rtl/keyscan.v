/*
 * keyscan.v
 *
 * vim: ts=4 sw=4
 *
 * Copyright (C) 2021  Piotr Esden-Tempski <piotr@esden.net>
 * SPDX-License-Identifier: CERN-OHL-P-2.0
 */

`default_nettype none

module keyscan (
    // KeyMatrix
	input wire [11:0] km_col,
	output wire [3:0] km_row,

	// Wishbone slave
	input  wire [ 2:0] wb_addr,
	output reg  [31:0] wb_rdata,
	input  wire [31:0] wb_wdata,
	input  wire        wb_we,
	input  wire        wb_cyc,
	output wire        wb_ack,

	// Clock / Reset
	input  wire clk,
	input  wire rst
);

	// Signals
	// -------

	// Wishbone
	reg  b_ack;
	reg  b_we_csr;
	wire b_rd_rst;

    // CSR
    reg [31:0] ks_csr;
    reg [11:0] ks_row [0:3];


	// Wishbone interface
	// ------------------

	// Ack
	always @(posedge clk)
		b_ack <= wb_cyc & ~b_ack;

	assign wb_ack = b_ack;

	// Write
	always @(posedge clk)
	begin
		if (b_ack) begin
			b_we_csr    <= 1'b0;
		end else begin
			b_we_csr    <= wb_cyc & wb_we & (wb_addr == 3'b000);
		end
	end

	always @(posedge clk)
		if (rst)
		    ks_csr <= 0;
		else if (b_we_csr)
			ks_csr <= wb_wdata;


	// Read
	assign b_rd_rst = ~wb_cyc | b_ack;

	always @(posedge clk) begin
		if (b_rd_rst)
			wb_rdata <= 32'h00000000;
		else
            casez (wb_addr)
			    3'b0zz: wb_rdata <= ks_csr;
                3'b1zz: wb_rdata <= ks_row[wb_addr[1:0]];
				default: wb_rdata <= 32'hxxxxxxxx;
            endcase
    end

    // Keyscanner
    reg [14:0] ks_div;

    always @(posedge clk) begin
        if (rst)
            ks_div <= 0;
        else
            // increment ks_div, if the HSB of ks_div is 0, otherwise reset to 0
            ks_div <= (ks_div + 1) & {($left(ks_div)+1){~ks_div[$left(ks_div)]}};
    end

    reg [1:0] ks_row_cnt;
    always @(posedge clk) begin
        if (rst) begin
            ks_row[0] <= 0;
            ks_row[1] <= 0;
            ks_row[2] <= 0;
            ks_row[3] <= 0;
        end else if (ks_div[14]) begin
            ks_row[ks_row_cnt] <= ~km_col;
            ks_row_cnt <= ks_row_cnt + 1;
        end
    end

    assign km_row = ~(4'b0001 << ks_row_cnt);


endmodule // keyscan
