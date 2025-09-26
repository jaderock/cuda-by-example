package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_library")

WIN32_LINKOPTS = [
  "-DEFAULTLIB:user32.lib",
  "-DEFAULTLIB:gdi32.lib",
  "-DEFAULTLIB:shell32.lib",
]

cc_library(
    name = "glfw-helper",
    srcs = [
      ":glfw_helper.cc",
    ],
    hdrs = [":glfw_helper.h"],
    linkopts = select({
        "@bazel_tools//src/conditions:windows": WIN32_LINKOPTS,
        #"@bazel_tools//src/conditions:linux_x86_64": LINUX_LINKOPTS,
        #"@bazel_tools//src/conditions:darwin": DARWIN_LINKOPTS,
    }),
    deps = [
        "//third-party/glad_opengl_v4_6:glad-lib",
    ] + select({
        "@bazel_tools//src/conditions:windows": [
            "@glfw-prebuilt-win64//:glfw-static",
        ],
        #"@bazel_tools//src/conditions:linux_x86_64": [":glfw_src"],
        #"@bazel_tools//src/conditions:darwin": [":glfw_src_darwin"],
    }),
)