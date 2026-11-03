//
// System Verilog Examples For Syntax Highlighting
// Taken from a hardware rasterizer project.
//
// Copyright (c) 2026 Samuel Fitzsimons (rainbain)
// SPDX-License-Identifier: MIT
//

`default_nettype none
`timescale 1ns/1ps

module raster #(
    parameter int ATTR_MAX_BITS = 32,

    // Example color generation
    // 1 sign bit, 8 color bits, 6 fractional bits
    parameter int NUM_ATTRS = 3,
    parameter int ATTR_BITS [NUM_ATTRS] = '{15, 15, 15},
    parameter int ATTR_FRAC [NUM_ATTRS] = '{6, 6, 6}
) (
    input logic clk,
    input logic rst_n,

    input  logic cmd_valid,
    output logic cmd_ready,

    input  logic       cmd_pipe_cmd,
    input  logic[7:0]  cmd_pipe,
    input  logic[7:0]  cmd_fmt,
    input  logic[4:0]  cmd_minx,
    input  logic[4:0]  cmd_miny,
    input  logic[4:0]  cmd_maxx,
    input  logic[4:0]  cmd_maxy,
    input  logic[17:0] cmd_w0_i,
    input  logic[17:0] cmd_w1_i,
    input  logic[17:0] cmd_w2_i,
    input  logic[9:0] cmd_dw0_dx,
    input  logic[9:0] cmd_dw1_dx,
    input  logic[9:0] cmd_dw2_dx,
    input  logic[9:0] cmd_dw0_dy,
    input  logic[9:0] cmd_dw1_dy,
    input  logic[9:0] cmd_dw2_dy,

    input logic [NUM_ATTRS-1:0][ATTR_MAX_BITS-1:0] cmd_attr_i,
    input logic [NUM_ATTRS-1:0][ATTR_MAX_BITS-1:0] cmd_attr_dx,
    input logic [NUM_ATTRS-1:0][ATTR_MAX_BITS-1:0] cmd_attr_dy,


    input  logic       frag_ready,
    output logic       frag_valid,

    output logic[4:0]  frag_x,
    output logic[4:0]  frag_y,
    output logic[7:0]  frag_fmt,

    output logic [NUM_ATTRS-1:0][ATTR_MAX_BITS-1:0] frag_attr,

    output logic       pipe_valid,
    input  logic       pipe_ready,
    output logic[7:0]  pipe
);
    logic[7:0]  fmt;

    // Tile and bounding box
    logic[4:0]  tile_x;
    logic[4:0]  tile_y;
    logic[4:0]  minx;
    logic[4:0]  maxx;
    logic[4:0]  maxy;

    // Edge Function Registers
    logic signed [17:0] w0_i;
    logic signed [17:0] w1_i;
    logic signed [17:0] w2_i;
    logic signed [17:0] w0;
    logic signed [17:0] w1;
    logic signed [17:0] w2;
    logic signed [9:0] dw0_dx;
    logic signed [9:0] dw1_dx;
    logic signed [9:0] dw2_dx;
    logic signed [9:0] dw0_dy;
    logic signed [9:0] dw1_dy;
    logic signed [9:0] dw2_dy;

    // Attribute Registers
    logic signed [NUM_ATTRS-1:0][ATTR_MAX_BITS-1:0] attr_i;
    logic signed [NUM_ATTRS-1:0][ATTR_MAX_BITS-1:0] attr;
    logic signed [NUM_ATTRS-1:0][ATTR_MAX_BITS-1:0] attr_dx;
    logic signed [NUM_ATTRS-1:0][ATTR_MAX_BITS-1:0] attr_dy;

    // Adder outputs
    logic signed [17:0] w0_next;
    logic signed [17:0] w1_next;
    logic signed [17:0] w2_next;
    logic signed [NUM_ATTRS-1:0][ATTR_MAX_BITS-1:0] attr_next;

    logic               raster_active;
    logic               vsync;
    logic               generate_output;

    always_ff @(posedge clk) begin
        if(cmd_valid && cmd_ready) begin
            raster_active <= !cmd_pipe_cmd;
            pipe_valid <= cmd_pipe_cmd;
            pipe <= cmd_pipe;
            cmd_ready <= 0;
            tile_x <= cmd_minx;
            tile_y <= cmd_miny;
            minx <= cmd_minx;
            maxx <= cmd_maxx;
            maxy <= cmd_maxy;
            w0_i <= cmd_w0_i;
            w1_i <= cmd_w1_i;
            w2_i <= cmd_w2_i;
            w0 <= cmd_w0_i;
            w1 <= cmd_w1_i;
            w2 <= cmd_w2_i;
            dw0_dx <= cmd_dw0_dx;
            dw1_dx <= cmd_dw1_dx;
            dw2_dx <= cmd_dw2_dx;
            dw0_dy <= cmd_dw0_dy;
            dw1_dy <= cmd_dw1_dy;
            dw2_dy <= cmd_dw2_dy;
            attr_i <= cmd_attr_i;
            attr <= cmd_attr_i;
            attr_dx <= cmd_attr_dx;
            attr_dy <= cmd_attr_dy;
            fmt <= cmd_fmt;
            vsync <= 0;
        end

        if(pipe_ready && pipe_valid) begin
            pipe_valid <= 0;
            cmd_ready <= 1;
        end

        // Clear valid? May be overridden
        if(frag_valid && frag_ready) begin
            frag_valid <= 0;
        end

        if(vsync) begin
            vsync <= 0;

            w0_i <= w0_next;
            w1_i <= w1_next;
            w2_i <= w2_next;
            w0 <= w0_next;
            w1 <= w1_next;
            w2 <= w2_next;

            attr <= attr_next;
            attr_i <= attr_next;
        end

        if(raster_active && (!frag_valid || frag_ready) && !vsync) begin
            // Step X Counters
            w0 <= w0_next;
            w1 <= w1_next;
            w2 <= w2_next;

            attr <= attr_next;

            tile_x <= tile_x + 1;
            if(tile_x == maxx) begin
                tile_x <= minx;
                tile_y <= tile_y + 1;
                if(tile_y == maxy) begin
                    // Last pixel
                    cmd_ready <= 1;
                    raster_active <= 0;
                end else begin
                    // Go down a row of pixels
                    vsync <= 1;
                end
            end
        end

        if(generate_output) begin
            frag_x <= tile_x;
            frag_y <= tile_y;
            frag_fmt <= fmt;
            frag_valid <= 1;
        end

        if(!rst_n) begin
            cmd_ready <= 1;
            frag_valid <= 0;
            raster_active <= 0;
            pipe_valid <= 0;
            vsync <= 0;
        end
    end

    always_comb begin
        generate_output = raster_active && (!frag_valid || frag_ready) && !vsync && w0 >= 0 && w1 >= 0 && w2 >= 0;

        w0_next = (vsync ? w0_i : w0) + (vsync ? 18'(dw0_dy) : 18'(dw0_dx));
        w1_next = (vsync ? w1_i : w1) + (vsync ? 18'(dw1_dy) : 18'(dw1_dx));
        w2_next = (vsync ? w2_i : w2) + (vsync ? 18'(dw2_dy) : 18'(dw2_dx));

        // Each attribute adder
        for (int i = 0; i < NUM_ATTRS; i++) begin
            attr_next[i] = (vsync ? attr_i[i] : attr[i]) + (vsync ? attr_dy[i] : attr_dx[i]);
        end
    end

    // Connect the attribute output
    // Only connecting the used bits
    // So that the unused adder bits get optimized out
    generate
        for (genvar i = 0; i < NUM_ATTRS; i++) begin : gen_attr
            always_comb begin
                frag_attr[i][ATTR_MAX_BITS-1:ATTR_BITS[i]] = '0;
            end

            always_ff @(posedge clk) begin
                if(generate_output) begin
                    frag_attr[i][ATTR_BITS[i]-1:0] <= attr[i][ATTR_BITS[i]-1:0];
                end
            end
        end
    endgenerate
endmodule

`define HALF_PERIOD 5

//
// Some components of the testbench for syntax highlighting of other system verilog features
module raster_tb;
    function automatic void show_cmd(input int t);
        if (cmd_log[t % MAX_CMD].pipe_cmd)
            $display("  command %0d: pipe token %h", t, cmd_log[t % MAX_CMD].pipe);
        else
            $display("  command %0d: triangle a=(%0d,%0d) b=(%0d,%0d) c=(%0d,%0d), tile at (%0d,%0d)", t,
                     cmd_vtx[t % MAX_CMD][0], cmd_vtx[t % MAX_CMD][1], cmd_vtx[t % MAX_CMD][2],
                     cmd_vtx[t % MAX_CMD][3], cmd_vtx[t % MAX_CMD][4], cmd_vtx[t % MAX_CMD][5],
                     cmd_tile[t % MAX_CMD][0], cmd_tile[t % MAX_CMD][1]);
    endfunction

    task automatic add_random_triangle();
        int ax, ay, bx, by, cx, cy;
        int pick;
        int skip;
        do begin
            ax = int'($urandom_range(SCREEN_W - 1));
            ay = int'($urandom_range(SCREEN_H - 1));
            pick = int'($urandom_range(99));
            if (pick < 25) begin
                // anywhere on screen
                bx = int'($urandom_range(SCREEN_W - 1));  by = int'($urandom_range(SCREEN_H - 1));
                cx = int'($urandom_range(SCREEN_W - 1));  cy = int'($urandom_range(SCREEN_H - 1));
            end else if (pick < 35) begin
                // huge, near three of the four screen corners, for the largest weights
                skip = int'($urandom_range(3));
                ax = near(CORNER_X[(skip + 1) % 4], 20, SCREEN_W - 1);
                ay = near(CORNER_Y[(skip + 1) % 4], 20, SCREEN_H - 1);
                bx = near(CORNER_X[(skip + 2) % 4], 20, SCREEN_W - 1);
                by = near(CORNER_Y[(skip + 2) % 4], 20, SCREEN_H - 1);
                cx = near(CORNER_X[(skip + 3) % 4], 20, SCREEN_W - 1);
                cy = near(CORNER_Y[(skip + 3) % 4], 20, SCREEN_H - 1);
            end else if (pick < 55) begin
                // small
                bx = near(ax, 8, SCREEN_W - 1);  by = near(ay, 8, SCREEN_H - 1);
                cx = near(ax, 8, SCREEN_W - 1);  cy = near(ay, 8, SCREEN_H - 1);
            end else if (pick < 70) begin
                // medium, about a tile
                bx = near(ax, 60, SCREEN_W - 1);  by = near(ay, 60, SCREEN_H - 1);
                cx = near(ax, 60, SCREEN_W - 1);  cy = near(ay, 60, SCREEN_H - 1);
            end else if (pick < 85) begin
                // sliver: two vertices next to each other
                bx = int'($urandom_range(SCREEN_W - 1));  by = int'($urandom_range(SCREEN_H - 1));
                cx = near(bx, 1, SCREEN_W - 1);           cy = near(by, 1, SCREEN_H - 1);
            end else begin
                // vertices on or next to tile boundaries
                ax = on_tile_edge(SCREEN_W - 1);  ay = on_tile_edge(SCREEN_H - 1);
                bx = on_tile_edge(SCREEN_W - 1);  by = on_tile_edge(SCREEN_H - 1);
                cx = on_tile_edge(SCREEN_W - 1);  cy = on_tile_edge(SCREEN_H - 1);
            end
        end while (tri_area(ax, ay, bx, by, cx, cy) == 0);
        add_tri_random_tile(ax, ay, bx, by, cx, cy, 24'($urandom), 24'($urandom), 24'($urandom));
    endtask


    // Sized literals in each base, a typedef and a case statement
    localparam logic [7:0] HEX = 8'hA5;
    localparam logic [3:0] BIN = 4'b10xz;
    localparam logic [5:0] OCT = 6'o17;
    localparam logic [7:0] DEC = 8'd200;

    typedef enum logic [1:0] {IDLE, RUN, DONE} state_t;
    state_t state;
    logic   clk = 0;
    logic   rst_n;

    /* Instance with named port connections */
    raster #(.NUM_ATTRS(3)) dut (
        .clk   (clk),
        .rst_n (rst_n)
    );

    initial forever #`HALF_PERIOD clk = ~clk;

    always_ff @(posedge clk) begin
        unique case (state)
            IDLE:    state <= RUN;
            RUN:     state <= DONE;
            default: state <= IDLE;
        endcase
    end
endmodule
