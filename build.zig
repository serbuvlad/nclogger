const std = @import("std");
const Builder = std.build.Builder;

pub fn build(b: *Builder) void {
    const exe = b.addExecutable("test", null);
    exe.addCSourceFile("test.c", &[_][]const u8{"-std=c99"});
    exe.linkLibC();
    exe.linkLibCpp();

    b.default_step.dependOn(&exe.step);

    const run_cmd = exe.run();
    const run_step = b.step("run", "Run the program");
    run_step.dependOn(&run_cmd.step);
}
