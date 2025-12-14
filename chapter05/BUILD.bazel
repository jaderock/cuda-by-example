load("@rules_cuda//cuda:defs.bzl", "cuda_library", "cuda_binary")

cuda_binary(
    name = "add_loop_blocks",
    srcs = ["add_loop_blocks.cu"],
)

cuda_binary(
    name = "add_loop_long_blocks",
    srcs = ["add_loop_long_blocks.cu"],
)

cuda_binary(
    name = "cuda_dot",
    srcs = [":dot.cu"],
)

cuda_binary(
    name = "shared_bitmap",
    srcs = [":shared_bitmap.cu"],
    deps = [
        "//common:glfw-helper",
        "//third-party/glad_opengl_v4_6:glad-lib",
    ],
)
