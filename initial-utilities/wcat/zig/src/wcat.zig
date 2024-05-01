const std = @import("std");
const stdout = std.io.getStdOut().writer();

pub fn main() !void {
    const exit_code = try wcat();
    if (exit_code != 0) {
        std.process.exit(exit_code);
    }
}

fn wcat() !u8 {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const alloc = arena.allocator();

    var args = std.process.argsWithAllocator(alloc) catch {
        try stdout.print("wcat: cannot read args\n", .{});
        return 1;
    };
    _ = args.next(); // Skip the program name.
    while (args.next()) |arg| {
        const file = std.fs.cwd().openFile(arg, .{}) catch {
            try stdout.print("wcat: cannot open file\n", .{});
            return 1;
        };
        defer file.close();

        var buffer: [4096]u8 = undefined;
        var bytes_num = file.read(buffer[0..]) catch {
            try stdout.print("wcat: cannot read file\n", .{});
            return 1;
        };
        while (bytes_num != 0) {
            try stdout.print("{s}", .{buffer[0..bytes_num]});
            bytes_num = file.read(buffer[0..]) catch {
                try stdout.print("wcat: cannot read file\n", .{});
                return 1;
            };
        }
    }
    return 0;
}
