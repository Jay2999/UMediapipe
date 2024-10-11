#include "Graphs.h"
#include <string>

const std::string cpu_graph = "input_stream: \"input_video\"\ninput_side_packet: \"MODEL_BLOB_HAND:hand_landmark_model\"\ninput_side_packet: \"MODEL_BLOB_PALM:palm_detection_model\"\ninput_side_packet: \"NUM_HANDS:num_hands\"\noutput_stream: \"landmarks\"\nnode {\ncalculator: \"HandLandmarkTrackingCpu\"\ninput_stream: \"IMAGE:input_video\"\ninput_side_packet: \"NUM_HANDS:num_hands\"\ninput_side_packet: \"MODEL_BLOB_HAND:hand_landmark_model\"\ninput_side_packet: \"MODEL_BLOB_PALM:palm_detection_model\"\noutput_stream: \"LANDMARKS:landmarks\"\noutput_stream: \"HANDEDNESS:handedness\"\noutput_stream: \"PALM_DETECTIONS:multi_palm_detections\"\n}";

mediapipe::CalculatorGraphConfig getHandLandmarkGraphConfigCpu() {
    return mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(cpu_graph);
}