#include "../include/UMediapipe.h"
#include "TfliteModels2.h"
#include "Debug.h"

#include "mediapipe/tasks/cc/vision/gesture_recognizer/gesture_recognizer.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"

namespace ump {
    using namespace mediapipe::tasks::vision::gesture_recognizer;

    std::unique_ptr<GestureRecognizer> graph = nullptr;
    HandLandmarksCallback handCallback = nullptr;

    void recognizer_callback(absl::StatusOr<GestureRecognizerResult> result, const mediapipe::Image &image, int64_t timestamp) {
        if (!result.ok()) {
            return;
        }
        auto dataVec = result->hand_landmarks;
        HandLandmarks landmarks[2];
        for (int j = 0; j < 2 && j < dataVec.size(); ++j) {
            for (int i = 0; i < 21 && dataVec[j].landmark_size() > 0; ++i) {
                Landmark& landmark = landmarks[j][i];
                landmark.x() = dataVec[j].landmark(i).x();
                landmark.y() = dataVec[j].landmark(i).y();
                landmark.z() = dataVec[j].landmark(i).z();
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
                landmarks[0].getHandedness() = Handedness::LEFT;
                landmarks[1].getHandedness() = Handedness::RIGHT;
            } else {
                landmarks[0].getHandedness() = Handedness::RIGHT;
                landmarks[1].getHandedness() = Handedness::LEFT;
            }
            handCallback(&landmarks[0]);
            handCallback(&landmarks[1]);

        // One hand
        } else if (result->handedness.size() > 0) {
            const std::string& label = result->handedness[0].classification(0).label();
            if (label == "Left") {
                landmarks[0].getHandedness() = Handedness::LEFT;
            } else {
                landmarks[0].getHandedness() = Handedness::RIGHT;
            }
            handCallback(&landmarks[0]);
        }
    }

    UMP_API UMediapipe::UMediapipe(HandLandmarksCallback handCallbackParam, unsigned int frameWidth, unsigned int frameHeight) :
            frameWidth(frameWidth), frameHeight(frameHeight) {
        handCallback = handCallbackParam;
    }

    UMP_API UMediapipe::~UMediapipe() {
        stopLandmarkDetection();
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

    UMP_API void UMediapipe::beginLandmarkDetection() {
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

    UMP_API void UMediapipe::stopLandmarkDetection() {
        if (graph) {
            graph->Close();
        }
        graph = nullptr;
    }

}