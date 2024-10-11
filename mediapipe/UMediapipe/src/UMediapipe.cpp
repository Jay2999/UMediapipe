#include "../include/UMediapipe.h"

#include <cstdlib>
#include <thread>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/absl_log.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/port/status.h"

#include "TfliteModels.h"
#include "Graphs.h"

constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "landmarks";

static mediapipe::CalculatorGraph graph;
static std::thread* grabberThread;
static volatile bool grab_frames;

static void grabFrames() {
    ABSL_LOG(INFO) << "Initialize the camera or load the video.";
    cv::VideoCapture capture;
    capture.open(0);
    if (!capture.isOpened()) {
        ABSL_LOG(ERROR) << "Stream not opened.";
        return;
    }

    ABSL_LOG(INFO) << "Start grabbing and processing frames.";
    while (grab_frames) {
        // Capture opencv camera or video frame.
        cv::Mat camera_frame_raw;
        capture >> camera_frame_raw;
        if (camera_frame_raw.empty()) {
            ABSL_LOG(INFO) << "Ignore empty frames from camera.";
            continue;
        }
        cv::Mat camera_frame;
        cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
        cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);

        // Wrap Mat into an ImageFrame.
        auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
            mediapipe::ImageFormat::SRGB, camera_frame.cols, camera_frame.rows,
            mediapipe::ImageFrame::kDefaultAlignmentBoundary
        );
        cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
        camera_frame.copyTo(input_frame_mat);

        // Send image packet into the graph.
        size_t frame_timestamp_us = (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
        graph.AddPacketToInputStream(kInputStream, mediapipe::Adopt(input_frame.release()).At(mediapipe::Timestamp(frame_timestamp_us)));
    }
    ABSL_LOG(INFO) << "Shutting down.";
    graph.CloseInputStream(kInputStream);
}

UMP_API void beginLandmarkDetection(HandLandmarksCallback callback) {
    mediapipe::CalculatorGraphConfig config = getHandLandmarkGraphConfigCpu();

    ABSL_LOG(INFO) << "Initialize the calculator graph.";
    ABSL_LOG(INFO) << graph.Initialize(config);

    ABSL_LOG(INFO) << "Start running the calculator graph.";

    // Set callback
    graph.ObserveOutputStream(kOutputStream,
        [callback](const mediapipe::Packet& packet) -> mediapipe::Status {
    		auto& data = packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
    		HandLandmarks landmarks;
    		for (int i = 0; i < 21 && data.size() > 0; ++i) {
    		    Landmark& landmark = landmarks.values[i];
    		    landmark.x() = data[0].landmark(i).x();
    		    landmark.y() = data[0].landmark(i).y();
    		    landmark.z() = data[0].landmark(i).z();
    		}
    		if (callback != 0x0) {
    		    callback(&landmarks);
    		}
    		return mediapipe::Status();
        }
    );
    std::map<std::string, mediapipe::Packet> extra_side_packets;
    extra_side_packets["hand_landmark_model"] = mediapipe::PointToForeign<std::string>(&models::hand_landmark_model);
    extra_side_packets["palm_detection_model"] = mediapipe::PointToForeign<std::string>(&models::palm_detection_model);
    extra_side_packets["num_hands"] = mediapipe::MakePacket<int>(2);
    ABSL_LOG(INFO) << graph.StartRun(extra_side_packets);
    grab_frames = true;
    grabberThread = new std::thread(grabFrames);
}

UMP_API void stopLandmarkDetection() {
    grab_frames = false;
}

UMP_API void waitForEnd() {
    grabberThread->join();
    delete grabberThread;
    grabberThread = nullptr;
    graph.WaitUntilDone();
}
