package(default_visibility = ["//visibility:public"])

cc_import(
    name = "UMediapipe_shared",
    hdrs = [
        "//mediapipe/UMediapipe/include:UMediapipe.h",
        "//mediapipe/UMediapipe/include:Types.h",
        "//mediapipe/UMediapipe/include:Platform.h",
    ],
    interface_library = "//bazel-bin/mediapipe/UMediapipe:UMediapipe.if.lib",
    shared_library = "//bazel-bin/mediapipe/UMediapipe:UMediapipe.dll",
)

cc_binary(
    name = "ump_desktop_example",
    srcs = ["main.cpp"],
    deps = [
        ":UMediapipe_shared",
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_video",
    ],
)
