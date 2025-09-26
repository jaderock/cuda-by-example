load("@rules_cuda//cuda:defs.bzl", "cuda_library", "cuda_binary")

cuda_binary(
    name = "vector-sum",
    srcs = ["vector-sum.cc"],
)

cuda_binary(
    name = "julia-gpu",
    srcs = ["julia-gpu.cc"],
    deps = [
        "//common:glfw-helper",
        "//third-party/glad_opengl_v4_6:glad-lib",
    ],
)
