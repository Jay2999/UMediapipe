#include "../include/UMediapipe.h"

#include <cstdlib>
#include <thread>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/classification.pb.h"
#include "mediapipe/framework/port/status.h"

#include "TfliteModels.h"
#include "Graphs.h"
#include "Debug.h"

constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "landmarks";

static mediapipe::CalculatorGraph graph;
static bool isGraphRunning = false;

namespace UMediapipe {

    UMediapipe::UMediapipe(HandLandmarksCallback leftHandCallback,
                           HandLandmarksCallback rightHandCallback,
                           unsigned int framwWidth, unsigned int frameHeight) :
            leftHandCallback(leftHandCallback), rightHandCallback(rightHandCallback),
            frameWidth(framwWidth), frameHeight(frameHeight) {}

    void UMediapipe::sendFrame(unsigned char *data) {
        if (!isGraphRunning) {
            DEBUG("Send frame was called but graph is not running.");
            return;
        }
        auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
                mediapipe::ImageFormat::SRGB, frameWidth, frameHeight,
                mediapipe::ImageFrame::kDefaultAlignmentBoundary
        );
        input_frame->CopyPixelData(mediapipe::ImageFormat::SRGB, frameWidth, frameHeight, data,
                                   mediapipe::ImageFrame::kDefaultAlignmentBoundary);

        size_t frame_timestamp_us =
                (double) cv::getTickCount() / (double) cv::getTickFrequency() * 1e6;
        graph.AddPacketToInputStream(kInputStream, mediapipe::Adopt(input_frame.release()).At(
                mediapipe::Timestamp(frame_timestamp_us)));
    }

    void UMediapipe::beginLandmarkDetection() {
    #if ANDROID
        mediapipe::CalculatorGraphConfig config = getHandLandmarkGraphConfigGpu();
    #elif WINDOWS
        mediapipe::CalculatorGraphConfig config = getHandLandmarkGraphConfigCpu();
    #endif
        DEBUG("Initialize the calculator graph.");
        std::string s = graph.Initialize(config).ToString();
        DEBUG(s.c_str());

        // Set callback
        graph.ObserveOutputStream(
                kOutputStream,
                [this](const mediapipe::Packet &packet) -> mediapipe::Status {
                    // Copy data
                    auto& data = packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
                    HandLandmarks landmarks[2];
                    for (int i = 0; i < 21 && data.size() > 0; ++i) {
                        for (int j = 0; j < 2 && j < data.size(); ++j) {
                            Landmark &landmark = landmarks[j][i];
                            landmark.x() = data[0].landmark(i).x();
                            landmark.y() = data[0].landmark(i).y();
                            landmark.z() = data[0].landmark(i).z();
                        }
                    }
                    leftHandCallback(&landmarks[0]);
                    /*
                    // Call appropriate callback
                    auto& classifications = packet.Get<std::vector<mediapipe::ClassificationList>>();
                    if (classifications.size() > 0) {
                        auto& cls0 = classifications[0];
                        std::string label0 = "";
                        if (cls0.classification(0).score() > cls0.classification(1).score()) {
                            label0 = cls0.classification(0).label();
                        }
                        else {
                            label0 = cls0.classification(1).label();
                        }

                        if (label0 == "Left") {
                            leftHandCallback(&landmarks[0]); // Left hand at 0
                        } else {
                            rightHandCallback(&landmarks[0]); // Right hand at 0
                        }

                        if (classifications.size() > 1) {
                            auto& cls1 = classifications[1];
                            std::string label1 = "";
                            if (cls1.classification(0).score() > cls1.classification(1).score()) {
                                label1 = cls1.classification(0).label();
                            }
                            else {
                                label1 = cls1.classification(1).label();
                            }
                            if (label1 != label0) {
                                if (label1 == "Left") {
                                    leftHandCallback(&landmarks[1]); // Left hand at 1
                                } else {
                                    rightHandCallback(&landmarks[1]); // Right hand at 1
                                }
                            }
                        }
                    }
                    */
                    return mediapipe::Status();
                }
        );

        DEBUG("Start running the calculator graph.");
        std::map<std::string, mediapipe::Packet> extra_side_packets;
        extra_side_packets["hand_landmark_model"] = mediapipe::PointToForeign<std::string>(
                &models::hand_landmark_model);
        extra_side_packets["palm_detection_model"] = mediapipe::PointToForeign<std::string>(
                &models::palm_detection_model);
        extra_side_packets["num_hands"] = mediapipe::MakePacket<int>(2);

        s = graph.StartRun(extra_side_packets).ToString();
        DEBUG(s.c_str());
        isGraphRunning = true;
    }

    void UMediapipe::stopLandmarkDetection() {
        if (isGraphRunning) {
            graph.CloseInputStream(kInputStream);
            graph.WaitUntilDone();
            isGraphRunning = false;
        }
    }

    UMediapipe::~UMediapipe() {
        stopLandmarkDetection();
    }
}