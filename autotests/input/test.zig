const std = @import("std");

/// This is a doc comment
// This is a comment
fn getData() !u32 {
    return 666;
}

// multiline string
const hello_world_in_c =
    \\#include <stdio.h>
    \\
    \\int main(int argc, char **argv) {
    \\    printf("hello world\n");
    \\    return 0;
    \\}
;

// Top-level declarations are order-independent:
const print = @import("std").debug.print;

pub fn main() !void {
    inline for (values) |v, i| {
        if (i != 2) continue;
        try expect(v);
    }

    // error union
    var number_or_error: anyerror!i32 = error.ArgNotFound;

    print("\nerror union 2\ntype: {}\nvalue: {!}\n", .{
        @TypeOf(number_or_error), number_or_error,
    });

    const stdout = std.io.getStdOut().writer();
    try stdout.print("Hello, {s}!\n", .{"world"});
    const bytes = "hello";
    print("{}\n", .{@TypeOf(bytes)}); // *const [5:0]u8
    print("{d}\n", .{bytes[5]}); // 0
    print("{}\n", .{'e' == '\x65'}); // true
    print("{d}\n", .{'\u{1f4a9}'});                     // 128169
    print("{d}\n", .{'💯'});                            // 128175
    print("{}\n", .{mem.eql(u8, "hello", "h\x65llo")}); // true
    print("0x{x}\n", .{"\xff"[0]}); // non-UTF-8 strings are possible with \xNN notation.
    print("{u}\n", .{'⚡'});

    _ = @import("introducing_zig_test.zig");
}

// Declare an enum.
const Type = enum {
    ok,
    not_ok,
};

test "async skip test" {
    var frame = async func();
    const result = await frame;
    try std.testing.expect(result == 1);
}

fn func() i32 {
    suspend {
        resume @frame();
    }
    return 1;
}

pub extern "c" fn @"error"() void;
pub extern "c" fn @"fstat$INODE64"(fd: c.fd_t, buf: *c.Stat) c_int;

const Color = enum {
  red,
  @"really red",
};
const color: Color = .@"really red";

var y: i32 = add(10, x);
const x: i32 = add(12, 34);

test "comptime vars" {
    var x: i32 = 1;
    comptime var y: i32 = 1;

    x += 1;
    y += 1;

    try expect(x == 2);
    try expect(y == 2);

    if (y != 2) {
        // This compile error never triggers because y is a comptime variable,
        // and so `y != 2` is a comptime value, and this if is statically evaluated.
        @compileError("wrong y value");
    }
}

const decimal_int = 98222;
const hex_int = 0xff;
const another_hex_int = 0xFF;
const octal_int = 0o755;
const binary_int = 0b11110000;

// underscores may be placed between two digits as a visual separator
const one_billion = 1_000_000_000;
const binary_mask = 0b1_1111_1111;
const permissions = 0o7_5_5;
const big_address = 0xFF80_0000_0000_0000;

const floating_point = 123.0E+77;
const another_float = 123.0;
const yet_another = 123.0e+77;

const hex_floating_point = 0x103.70p-5;
const another_hex_float = 0x103.70;
const yet_another_hex_float = 0x103.70P-5;

// underscores may be placed between two digits as a visual separator
const lightspeed = 299_792_458.000_000;
const nanosecond = 0.000_000_001;
const more_hex = 0x1234_5678.9ABC_CDEFp-10;

const A = error{One};
const B = error{Two};
(A || B) == error{One, Two}

const x: u32 = 1234;
const ptr = &x;
ptr.* == 1234

// get the size of an array
comptime {
    assert(message.len == 5);
}

test "iterate over an array" {
    var sum: usize = 0;
    for (message) |byte| {
        sum += byte;
    }
    try expect(sum == 'h' + 'e' + 'l' * 2 + 'o');
}

// use compile-time code to initialize an array
var fancy_array = init: {
    var initial_value: [10]Point = undefined;
    for (initial_value) |*pt, i| {
        pt.* = Point{
            .x = @intCast(i32, i),
            .y = @intCast(i32, i) * 2,
        };
    }
    break :init initial_value;
};

test "switch on non-exhaustive enum" {
    try expect(result);
    const is_one = switch (number) {
        .one => true,
        else => false,
    };
    try expect(is_one);

    const array_ptr = array[0..array.len];

    if (a != b) {
        try expect(true);
    } else if (a == 9) {
        unreachable;
    } else {
        unreachable;
    }
}

fn deferErrorExample(is_error: bool) !void {
    print("\nstart of function\n", .{});

    // This will always be executed on exit
    defer {
        print("end of function\n", .{});
    }

    errdefer {
        print("encountered an error!\n", .{});
    }
}

pub fn syscall3(number: usize, arg1: usize, arg2: usize, arg3: usize) usize {
    return asm volatile ("syscall"
        : [ret] "={rax}" (-> usize)
        : [number] "{rax}" (number),
          [arg1] "{rdi}" (arg1),
          [arg2] "{rsi}" (arg2),
          [arg3] "{rdx}" (arg3)
        : "rcx", "r11"
    );
}
