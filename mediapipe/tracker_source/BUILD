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

cc_binary(
    name = "hand_tracking_stripped",
	srcs = ["hand_tracking_stripped_main.cpp"],
    data = [
        "//mediapipe/modules/hand_landmark:hand_landmark_lite.tflite",
        "//mediapipe/modules/palm_detection:palm_detection_lite.tflite",
    ],
    deps = [
        "//mediapipe/tracker_source:desktop_tflite_calculators",
		"//mediapipe/framework/port:opencv_highgui",
		"//mediapipe/framework/port:parse_text_proto",
		"@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/log:absl_log",
    ],
)