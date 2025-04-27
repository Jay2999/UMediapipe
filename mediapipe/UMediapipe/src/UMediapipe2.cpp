#include "../include/UMediapipe.h"
#include "TfliteModels2.h"
//#include "mediapipe/framework/port/opencv_calib3d_inc.h"
#include "opencv2/calib3d.hpp"
#include "Debug.h"

#include "mediapipe/tasks/cc/vision/gesture_recognizer/gesture_recognizer.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"

cv::Mat cameraMtx = (cv::Mat_<float>(3,3) << 0, 0, 0, 0, 0, 0, 0, 0, 1);;
cv::Mat distCoeffs = (cv::Mat_<float>(4, 1) << 0, 0, 0, 0);

template <typename T>
static cv::Mat landmarkListToMat(const T& landmarkList) {
    cv::Mat points(landmarkList.landmark_size(), 3, CV_32F);
    for (int i = 0; i < landmarkList.landmark_size(); ++i) {
        points.at<float>(i, 0) = landmarkList.landmark(i).x();
        points.at<float>(i, 1) = landmarkList.landmark(i).y();
        points.at<float>(i, 2) = landmarkList.landmark(i).z();
    }
    return points;
}

static bool to3DLandmarks(
        const mediapipe::LandmarkList& modelPoints,
        const mediapipe::NormalizedLandmarkList& imagePoints,
        const cv::Mat& cameraMatrix,
        const cv::Mat& distortion, ump::HandLandmarks& output) {
    cv::Mat mPoints = landmarkListToMat<mediapipe::LandmarkList>(modelPoints) * -1;
    cv::Mat iPoints = landmarkListToMat<mediapipe::NormalizedLandmarkList>(imagePoints);
    iPoints = iPoints.colRange(0, iPoints.cols - 1).clone();
    iPoints.col(0) *= cameraMatrix.at<float>(0, 2) * 2;
    iPoints.col(1) *= cameraMatrix.at<float>(1, 2) * 2;
    cv::Mat rotation_vector(3, 1, CV_32F);
    cv::Mat translation_vector(3, 1, CV_32F);
#if WINDOWS
    bool success = cv::solvePnP(
            mPoints, iPoints, cameraMatrix, distortion,
            rotation_vector, translation_vector, 0
    );
    //std::cout << mPoints << std::endl << iPoints << std::endl;
#elif ANDROID
    bool success = cv::solvePnP(
            mPoints, iPoints, cameraMatrix, distortion,
            rotation_vector, translation_vector, 8 //cv::SOLVEPNP_SQPNP
    );
#endif
    if (success) {
        cv::Mat transform = cv::Mat::eye(4, 4, CV_32F);
        transform.at<float>(0, 3) = translation_vector.at<float>(0, 0);
        transform.at<float>(1, 3) = translation_vector.at<float>(1, 0);
        transform.at<float>(2, 3) = translation_vector.at<float>(2, 0);
        for (int i = 0; i < modelPoints.landmark_size(); ++i) {
            const auto& lm = modelPoints.landmark(i);
            cv::Mat p = (cv::Mat_<float>(1, 4) << -lm.x(), -lm.y(), -lm.z(), 1);
            cv::Mat tp = p * transform.inv().t();
            output[i].x() = tp.at<float>(0, 0);
            output[i].y() = tp.at<float>(0, 1);
            output[i].z() = tp.at<float>(0, 2);
        }
    }
    return success;
}

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
        auto& normalized = result->hand_landmarks;
        auto& world = result->hand_world_landmarks;
        HandDetectionResult hands[2];
        for (int j = 0; j < 2 && j < dataVec.size(); ++j) {
#if 0
            HandLandmarks& landmarks = hands[j].Landmarks();
            if (!to3DLandmarks(world[j], normalized[j], cameraMtx, distCoeffs, landmarks))
                return;
#else
            for (int i = 0; i < 21 && dataVec[j].landmark_size() > 0; ++i) {
                Landmark& landmark = hands[j].Landmarks(i);
                landmark.x() = dataVec[j].landmark(i).x();
                landmark.y() = dataVec[j].landmark(i).y();
                landmark.z() = dataVec[j].landmark(i).z();
            }
#endif
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
        cameraMtx.at<float>(0, 0) = frameWidth * 0.75; // fx
        cameraMtx.at<float>(1, 1) = frameWidth * 0.75; // fy
        cameraMtx.at<float>(0, 2) = frameWidth / 2.0f; // cx
        cameraMtx.at<float>(1, 2) = frameHeight / 2.0f; // cy
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