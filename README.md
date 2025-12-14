# cuda-by-example
Sample CUDA projects for the CUDA by Example book

Helper functions are re-written to use modern C++ and frameworks such as GLFW, GLEW, etc.

## Original Code
Original code can be downloaded from the `CUDA by Example` book website at https://developer.nvidia.com/cuda-example

## Build
The projects are built with CUDA Toolkit v13.0 and Bazel v8.5.

Build -

`$ bazel build //chapter3:hello-world`

Run - 

`$ bazel run //chapter3:hello-world`

## Third Party Dependency

### GLFW

### GLAD

GLAD library is loaded from https://glad.dav1d.de/

GLAD bazel build file can be found at https://github.com/jaderock/bazel-build-library/tree/main/glad
