load(
    "//mediapipe/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
    "mediapipe_simple_subgraph",
)

package(default_visibility = ["//visibility:public"])

exports_files(glob([
    "*.pbtxt",
]))

mediapipe_simple_subgraph(
    name = "hand_landmark_model_loader",
    graph = "hand_landmark/hand_landmark_model_loader.pbtxt",
    register_as = "HandLandmarkModelLoader",
    deps = [
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/tflite:tflite_model_calculator",
        "//mediapipe/framework/tool:switch_container",
    ],
)

mediapipe_simple_subgraph(
    name = "palm_detection_model_loader",
    graph = "palm_detection/palm_detection_model_loader.pbtxt",
    register_as = "PalmDetectionModelLoader",
    deps = [
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/tflite:tflite_model_calculator",
        "//mediapipe/calculators/util:local_file_contents_calculator",
        "//mediapipe/framework/tool:switch_container",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_cpu",
    graph = "hand_landmark/hand_landmark_cpu.pbtxt",
    register_as = "HandLandmarkCpu",
    deps = [
        "//mediapipe/calculators/core:gate_calculator",
        "//mediapipe/calculators/core:split_vector_calculator",
        "//mediapipe/calculators/tensor:image_to_tensor_calculator",
        "//mediapipe/calculators/tensor:inference_calculator",
        "//mediapipe/calculators/tensor:tensors_to_classification_calculator",
        "//mediapipe/calculators/tensor:tensors_to_floats_calculator",
        "//mediapipe/calculators/tensor:tensors_to_landmarks_calculator",
        "//mediapipe/calculators/util:landmark_letterbox_removal_calculator",
        "//mediapipe/calculators/util:landmark_projection_calculator",
        "//mediapipe/calculators/util:thresholding_calculator",
        "//mediapipe/calculators/util:world_landmark_projection_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "palm_detection_cpu",
    graph = "palm_detection/palm_detection_cpu.pbtxt",
    register_as = "PalmDetectionCpu",
    deps = [
        "//mediapipe/calculators/tensor:image_to_tensor_calculator",
        "//mediapipe/calculators/tensor:inference_calculator",
        "//mediapipe/calculators/tensor:tensors_to_detections_calculator",
        "//mediapipe/calculators/tflite:ssd_anchors_calculator",
        "//mediapipe/calculators/tflite:tflite_custom_op_resolver_calculator",
        "//mediapipe/calculators/util:detection_letterbox_removal_calculator",
        "//mediapipe/calculators/util:non_max_suppression_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_tracking_cpu",
    graph = "hand_landmark/hand_landmark_tracking_cpu.pbtxt",
    register_as = "HandLandmarkTrackingCpu",
    deps = [
        ":hand_landmark_cpu",
        "//mediapipe/modules/hand_landmark:hand_landmark_landmarks_to_roi",
        "//mediapipe/modules/hand_landmark:palm_detection_detection_to_roi",
        "//mediapipe/calculators/core:begin_loop_calculator",
        "//mediapipe/calculators/core:clip_vector_size_calculator",
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/core:end_loop_calculator",
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/core:gate_calculator",
        "//mediapipe/calculators/core:previous_loopback_calculator",
        "//mediapipe/calculators/image:image_properties_calculator",
        "//mediapipe/calculators/util:association_norm_rect_calculator",
        "//mediapipe/calculators/util:collection_has_min_size_calculator",
        "//mediapipe/calculators/util:filter_collection_calculator",
        ":palm_detection_cpu",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_gpu",
    graph = "hand_landmark/hand_landmark_gpu.pbtxt",
    register_as = "HandLandmarkGpu",
    deps = [
        "//mediapipe/calculators/core:gate_calculator",
        "//mediapipe/calculators/core:split_vector_calculator",
        "//mediapipe/calculators/tensor:image_to_tensor_calculator",
        "//mediapipe/calculators/tensor:inference_calculator",
        "//mediapipe/calculators/tensor:tensors_to_classification_calculator",
        "//mediapipe/calculators/tensor:tensors_to_floats_calculator",
        "//mediapipe/calculators/tensor:tensors_to_landmarks_calculator",
        "//mediapipe/calculators/util:landmark_letterbox_removal_calculator",
        "//mediapipe/calculators/util:landmark_projection_calculator",
        "//mediapipe/calculators/util:thresholding_calculator",
        "//mediapipe/calculators/util:world_landmark_projection_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "palm_detection_gpu",
    graph = "palm_detection/palm_detection_gpu.pbtxt",
    register_as = "PalmDetectionGpu",
    deps = [
        "//mediapipe/calculators/tensor:image_to_tensor_calculator",
        "//mediapipe/calculators/tensor:inference_calculator",
        "//mediapipe/calculators/tensor:tensors_to_detections_calculator",
        "//mediapipe/calculators/tflite:ssd_anchors_calculator",
        "//mediapipe/calculators/tflite:tflite_custom_op_resolver_calculator",
        "//mediapipe/calculators/util:detection_letterbox_removal_calculator",
        "//mediapipe/calculators/util:non_max_suppression_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_tracking_gpu",
    graph = "hand_landmark/hand_landmark_tracking_gpu.pbtxt",
    register_as = "HandLandmarkTrackingGpu",
    deps = [
        ":hand_landmark_gpu",
        "//mediapipe/modules/hand_landmark:hand_landmark_landmarks_to_roi",
        "//mediapipe/modules/hand_landmark:palm_detection_detection_to_roi",
        "//mediapipe/calculators/core:begin_loop_calculator",
        "//mediapipe/calculators/core:clip_vector_size_calculator",
        "//mediapipe/calculators/core:constant_side_packet_calculator",
        "//mediapipe/calculators/core:end_loop_calculator",
        "//mediapipe/calculators/core:flow_limiter_calculator",
        "//mediapipe/calculators/core:gate_calculator",
        "//mediapipe/calculators/core:previous_loopback_calculator",
        "//mediapipe/calculators/image:image_properties_calculator",
        "//mediapipe/calculators/util:association_norm_rect_calculator",
        "//mediapipe/calculators/util:collection_has_min_size_calculator",
        "//mediapipe/calculators/util:filter_collection_calculator",
        "//mediapipe/gpu:image_frame_to_gpu_buffer_calculator",
        ":palm_detection_gpu",
    ],
)