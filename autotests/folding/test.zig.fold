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
pub fn main() !void {
    inline for (values) |v, i| {
        if (i != 2) continue;
        try expect(v);
    }
    const stdout = std.io.getStdOut().writer();
    try stdout.print("Hello, {s}!\n", .{"world"});
    const bytes = "hello";
    print("{}\n", .{@TypeOf(bytes)}); // *const [5:0]u8
    print("{d}\n", .{bytes[5]}); // 0
    print("{}\n", .{'e' == '\x65'}); // true
}
