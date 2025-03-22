#include "../include/UMediapipe.h"
#include "TfliteModels2.h"
#include "Debug.h"

#include "mediapipe/tasks/cc/vision/gesture_recognizer/gesture_recognizer.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"

namespace ump {
    using namespace mediapipe::tasks::vision::gesture_recognizer;

    std::unique_ptr<GestureRecognizer> graph = nullptr;
    HandDetectionCallback handCallback = nullptr;

    const static std::map<std::string, HandGestures> gesturesMap {
        {"None", HandGestures::NONE},
        {"Open_Palm", HandGestures::OPEN_PALM},
        {"Closed_Fist", HandGestures::CLOSED_FIST}
    };

    void recognizer_callback(absl::StatusOr<GestureRecognizerResult> result, const mediapipe::Image &image, int64_t timestamp) {
        if (!result.ok()) {
            std::string msg = result.status().ToString();
            DEBUG(msg.c_str());
            return;
        }
        auto dataVec = result->hand_landmarks;
        HandDetectionResult hands[2];
        for (int j = 0; j < 2 && j < dataVec.size(); ++j) {
            for (int i = 0; i < 21 && dataVec[j].landmark_size() > 0; ++i) {
                Landmark& landmark = hands[j].Landmarks(i);
                landmark.x() = dataVec[j].landmark(i).x();
                landmark.y() = dataVec[j].landmark(i).y();
                landmark.z() = dataVec[j].landmark(i).z();
            }
        }
        for (int i = 0; i < 2 && i < result->gestures.size(); ++i) {
            const std::string& gestureName = result->gestures[i].classification(0).label();
            if (gesturesMap.find(gestureName) != gesturesMap.end()) {
                HandGestures& gesture = hands[i].Gesture();
                gesture = gesturesMap.at(gestureName);
            }
        }

        // Two hands
        if (result->handedness.size() > 1) {
            std::string label1 = result->handedness[0].classification(0).label();
            std::string label2 = result->handedness[1].classification(0).label();
            if (label1 == label2) {
                const auto& score1 = result->handedness[0].classification(0).score();
                const auto& score2 = result->handedness[1].classification(0).score();
                if (score1 < score2) {
                    label1 = label1 == "Left" ? "Right" : "Left";
                } else {
                    label2 = label2 == "Left" ? "Right" : "Left";
                }
            }
            if (label1 == "Left") {
                hands[0].getHandedness() = Handedness::LEFT;
                hands[1].getHandedness() = Handedness::RIGHT;
            } else {
                hands[0].getHandedness() = Handedness::RIGHT;
                hands[1].getHandedness() = Handedness::LEFT;
            }
            handCallback(&hands[0]);
            handCallback(&hands[1]);

        // One hand
        } else if (result->handedness.size() > 0) {
            const std::string& label = result->handedness[0].classification(0).label();
            if (label == "Left") {
                hands[0].getHandedness() = Handedness::LEFT;
            } else {
                hands[0].getHandedness() = Handedness::RIGHT;
            }
            handCallback(&hands[0]);
        }
    }

    UMP_API UMediapipe::UMediapipe(HandDetectionCallback handCallbackParam, unsigned int frameWidth, unsigned int frameHeight) :
            frameWidth(frameWidth), frameHeight(frameHeight) {
        handCallback = handCallbackParam;
    }

    UMP_API UMediapipe::~UMediapipe() {
        stopHandDetection();
        handCallback = nullptr;
    }

    UMP_API void UMediapipe::sendFrame(unsigned char *data) {
        if (!graph) {
            DEBUG("Send frame was called but graph is not running.");
            return;
        }

        auto input_frame = std::make_shared<mediapipe::ImageFrame>(mediapipe::ImageFormat::SRGB, frameWidth, frameHeight, mediapipe::ImageFrame::kDefaultAlignmentBoundary);
        input_frame->CopyPixelData(mediapipe::ImageFormat::SRGB, frameWidth, frameHeight, data, mediapipe::ImageFrame::kDefaultAlignmentBoundary);
        mediapipe::Image input(input_frame);

        size_t frame_timestamp_us = (double) cv::getTickCount() / (double) cv::getTickFrequency() * 1e6;
        graph->RecognizeAsync(input, frame_timestamp_us);
    }

    UMP_API void UMediapipe::beginHandDetection() {
        auto options = std::make_unique<GestureRecognizerOptions>();
        options->running_mode = mediapipe::tasks::vision::core::RunningMode::LIVE_STREAM;
        options->num_hands = 2;
        options->result_callback = recognizer_callback;
        options->base_options.model_asset_buffer = std::make_unique<std::string>(models::gesture_recognizer_model);

#if WINDOWS
        options->base_options.delegate = mediapipe::tasks::core::BaseOptions::Delegate::CPU;
#else
        options->base_options.delegate = mediapipe::tasks::core::BaseOptions::Delegate::GPU;
#endif

        auto gestureRecognizer = GestureRecognizer::Create(std::move(options));
        if (gestureRecognizer.ok()) {
            graph = std::move(*gestureRecognizer);
        } else {
            std::string msg = gestureRecognizer.status().ToString();
            DEBUG(msg.c_str());
        }
    }

    UMP_API void UMediapipe::stopHandDetection() {
        if (graph) {
            graph->Close();
        }
        graph = nullptr;
    }

}