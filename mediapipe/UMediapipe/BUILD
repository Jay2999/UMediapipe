package(default_visibility = ["//visibility:public"])

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
    alwayslink=True,
)

cc_library(
    name = "desktop_tflite_calculators",
    deps = [
        ":desktop_offline_calculators",
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/core:merge_calculator",
        "//mediapipe/UMediapipe/graphs:hand_landmark_tracking_cpu",
        "//mediapipe/calculators/tflite:tflite_model_calculator"
    ],
    alwayslink=True,
)

cc_library(
    name = "mobile_calculators",
    deps = [
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/UMediapipe/graphs:hand_landmark_tracking_gpu",
    ],
    alwayslink = True,
)

cc_library(
    name = "UMediapipe_static",
	srcs = [
	    "src/hand_landmark_model_bin.cpp",
        "src/palm_detection_model_bin.cpp",
        "src/hand_tracking_graph.cpp",
	    "src/UMediapipe.cpp",
	    "src/TfliteModels.h",
	    "src/Graphs.h",
	    "src/Debug.h",
	    "//mediapipe/UMediapipe/include:UMediapipe.h",
	    "//mediapipe/UMediapipe/include:Platform.h",
	    "//mediapipe/UMediapipe/include:Types.h",
	],
    defines = [
        "DLL_EXPORT",
        "DEBUG_BUILD",
    ],
    deps = [
        ":desktop_tflite_calculators",
		"//mediapipe/framework/port:parse_text_proto",
		"@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/log:absl_log",
    ],
    alwayslink = True,
)

cc_library(
    name = "UMediapipe_static2",
    hdrs = [
        "//mediapipe/UMediapipe/include:UMediapipe.h",
        "//mediapipe/UMediapipe/include:Platform.h",
        "//mediapipe/UMediapipe/include:Types.h",
    ],
	srcs = [
        "src/gesture_recognizer_model_bin.cpp",
	    "src/UMediapipe2.cpp",
	    "src/TfliteModels2.h",
	    "src/Graphs.h",
	    "src/Debug.h",
	],
    defines = [
        "DLL_EXPORT",
        "DEBUG_BUILD",
    ],
    deps = [
        "//mediapipe/tasks/cc/vision/gesture_recognizer:gesture_recognizer",
    ],
    alwayslink = True,
)

config_setting(
    name = "arm_build",
    values = {"cpu": "arm64-v8a"},
)

cc_binary(
    name = "UMediapipe",
    deps = [
        ":UMediapipe_static2",
    ],
    linkshared = True,
)