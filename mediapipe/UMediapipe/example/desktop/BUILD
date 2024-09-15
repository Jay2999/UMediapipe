package(default_visibility = ["//visibility:public"])

exports_files(glob([
    "*.pbtxt",
]))

cc_library(
    name = "desktop_offline_calculators",
    deps = [
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/core:gate_calculator",
        "//mediapipe/calculators/core:immediate_mux_calculator",
        "//mediapipe/calculators/core:packet_inner_join_calculator",
        "//mediapipe/calculators/core:previous_loopback_calculator",
        "//mediapipe/calculators/video:opencv_video_decoder_calculator",
        "//mediapipe/calculators/video:opencv_video_encoder_calculator",
    ],
)

cc_library(
    name = "desktop_tflite_calculators",
    deps = [
        ":desktop_offline_calculators",
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/core:merge_calculator",
        "//mediapipe/modules/hand_landmark:hand_landmark_tracking_cpu",
    ],
)

cc_import(
    name = "UMediapipe_shared",
    data = [
        "//mediapipe/modules/hand_landmark:hand_landmark_lite.tflite",
        "//mediapipe/modules/palm_detection:palm_detection_lite.tflite",
        "//mediapipe/UMediapipe/example/desktop/graphs:hand_tracking_desktop_live_stripped.pbtxt",
    ],
    hdrs = ["//mediapipe/UMediapipe/include:UMediapipe.h"],
    interface_library = "//bazel-bin/mediapipe/UMediapipe:UMediapipe_shared.if.lib",
    shared_library = "//bazel-bin/mediapipe/UMediapipe:UMediapipe_shared.dll",
)

cc_binary(
    name = "ump_desktop_example",
    srcs = ["main.cpp"],
    deps = [
        ":UMediapipe_shared",
        "//third_party:opencv",
    ],
)
