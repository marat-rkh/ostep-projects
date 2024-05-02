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
        try stdout.print("wgrep: cannot read args\n", .{});
        return 1;
    };
    _ = args.next(); // Skip the program name.
    const search_term = args.next() orelse {
        try stdout.print("wgrep: searchterm [file ...]\n", .{});
        return 1;
    };
    var file_count: u8 = 0;
    while (args.next()) |arg| {
        file_count += 1;
        const file = std.fs.cwd().openFile(arg, .{}) catch {
            try stdout.print("wgrep: cannot open file\n", .{});
            return 1;
        };
        defer file.close();

        if (try wgrepFile(alloc, search_term, file.reader()) != 0) {
            return 1;
        }
    }
    if (file_count == 0) {
        return wgrepFile(alloc, search_term, std.io.getStdIn().reader());
    }
    return 0;
}

fn wgrepFile(alloc: std.mem.Allocator, search_term: []const u8, reader: std.fs.File.Reader) !u8 {
    var maybe_line = reader.readUntilDelimiterOrEofAlloc(alloc, '\n', 100 * 1024 * 1024) catch {
        try stdout.print("wgrep: cannot read file, too long line\n", .{});
        return 1;
    };
    while (maybe_line) |line| {
        if (std.mem.indexOf(u8, line, search_term) != null) {
            try stdout.print("{s}\n", .{line});
        }
        maybe_line = reader.readUntilDelimiterOrEofAlloc(alloc, '\n', 100 * 1024 * 1024) catch {
            try stdout.print("wgrep: cannot read file, too long line\n", .{});
            return 1;
        };
    }
    return 0;
}
