// Copyright 2019 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <cstdlib>
#include <string>

// #include "absl/flags/flag.h"
// #include "absl/flags/parse.h"
#include "mediapipe/framework/calculator_graph.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"

constexpr char kInputStream[] = "input_video";

struct Landmark {
  float x;
  float y;
  float z;
  float visibility;
  float presence;
};

class PoseGraph {
  public: 
    PoseGraph();
    bool process(const cv::Mat *input, Landmark output[33]);
    ~PoseGraph();
  private: 
    std::unique_ptr<mediapipe::OutputStreamPoller> poller;
    std::unique_ptr<mediapipe::CalculatorGraph> graph;
};

PoseGraph::PoseGraph() {
  const char* output_node = "pose_landmarks";
  const char* graph_config = R""""(
  input_stream: "input_video"
  output_stream: "pose_landmarks"

  node {
    calculator: "ConstantSidePacketCalculator"
    output_side_packet: "PACKET:enable_segmentation"
    node_options: {
      [type.googleapis.com/mediapipe.ConstantSidePacketCalculatorOptions]: {
        packet { bool_value: true }
      }
    }
  }

  node {
    calculator: "FlowLimiterCalculator"
    input_stream: "input_video"
    input_stream: "FINISHED:pose_landmarks"
    input_stream_info: {
      tag_index: "FINISHED"
      back_edge: true
    }
    output_stream: "throttled_input_video"
  }

  node {
    calculator: "PoseLandmarkCpu"
    input_side_packet: "ENABLE_SEGMENTATION:enable_segmentation"
    input_stream: "IMAGE:throttled_input_video"
    output_stream: "LANDMARKS:pose_landmarks"
    output_stream: "SEGMENTATION_MASK:segmentation_mask"
    output_stream: "DETECTION:pose_detection"
    output_stream: "ROI_FROM_LANDMARKS:roi_from_landmarks"
  }
)"""";

  mediapipe::CalculatorGraphConfig config = mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
      graph_config);

  graph = std::make_unique<mediapipe::CalculatorGraph>();
  
  absl::Status status = graph->Initialize(config);
  if (!status.ok()) {
    std::cerr << "Graph Init" << status.message() << std::endl;
    throw std::runtime_error("Failed to create the graph");
  }

  mediapipe::StatusOrPoller sop = graph->AddOutputStreamPoller(output_node);

  if (!sop.ok()) {
    std::cerr << "Failed to add poller to graph: [" << sop.status().message() << "]" << std::endl;
    throw std::runtime_error("Failed to add poller to graph");
  }

  assert(sop.ok());

  poller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(sop.value()));
  
  graph->StartRun({});
}

PoseGraph::~PoseGraph() {
  graph->CloseInputStream(kInputStream);
  graph->WaitUntilDone();
}

bool PoseGraph::process(const cv::Mat *input, Landmark output[33]) {
  auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
      mediapipe::ImageFormat::SRGB, input->cols, input->rows,
      mediapipe::ImageFrame::kDefaultAlignmentBoundary);
  
  cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
  input->copyTo(input_frame_mat);

  size_t frame_timestamp_us =
      (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;

  absl::Status run_status = graph->AddPacketToInputStream(
      kInputStream, mediapipe::Adopt(input_frame.release())
                        .At(mediapipe::Timestamp(frame_timestamp_us)));

  if (!run_status.ok()) {
    std::cerr << "Add Packet error: [" << run_status.message() << "]" << std::endl;
    return false;
  }

  mediapipe::Packet packet;
  if (poller->QueueSize() > 0) {
    if (poller->Next(&packet)) {
      const mediapipe::NormalizedLandmarkList &landmarks = packet.Get<mediapipe::NormalizedLandmarkList>();
      
      for (int idx = 0; idx < landmarks.landmark_size(); ++idx) { 
        const mediapipe::NormalizedLandmark& landmark = landmarks.landmark(idx);
    
        output[idx] = {
          .x = landmark.x(),
          .y = landmark.y(),
          .z = landmark.z(),
          .visibility = landmark.visibility(),
          .presence = landmark.presence(),
        };
      }
    } else {
      std::cerr << "No packet from poller" << std::endl; 
      return false;
    }
  }

  return true;
}

class FaceMeshGraph {
  public:
    FaceMeshGraph();
    bool process(const cv::Mat *input, Landmark output[478]);
    ~FaceMeshGraph();
  private:
    std::unique_ptr<mediapipe::OutputStreamPoller> poller;
    std::unique_ptr<mediapipe::CalculatorGraph> graph;
};

FaceMeshGraph::FaceMeshGraph() {
  const char* output_node = "multi_face_landmarks";
  const char* graph_config = R""""(
  input_stream: "input_video"
  output_stream: "multi_face_landmarks"

  node {
    calculator: "FlowLimiterCalculator"
    input_stream: "input_video"
    input_stream: "FINISHED:multi_face_landmarks"
    input_stream_info: {
      tag_index: "FINISHED"
      back_edge: true
    }
    output_stream: "throttled_input_video"
  }

  node {
    calculator: "ConstantSidePacketCalculator"
    output_side_packet: "PACKET:0:num_faces"
    output_side_packet: "PACKET:1:with_attention"
    node_options: {
      [type.googleapis.com/mediapipe.ConstantSidePacketCalculatorOptions]: {
        packet { int_value: 1 }
        packet { bool_value: true }
      }
    }
  }

  node {
    calculator: "FaceLandmarkFrontCpu"
    input_stream: "IMAGE:throttled_input_video"
    input_side_packet: "NUM_FACES:num_faces"
    input_side_packet: "WITH_ATTENTION:with_attention"
    output_stream: "LANDMARKS:multi_face_landmarks"
    output_stream: "ROIS_FROM_LANDMARKS:face_rects_from_landmarks"
    output_stream: "DETECTIONS:face_detections"
    output_stream: "ROIS_FROM_DETECTIONS:face_rects_from_detections"
  }
)"""";
  
  mediapipe::CalculatorGraphConfig config = mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
      graph_config);

  graph = std::make_unique<mediapipe::CalculatorGraph>();

  absl::Status status = graph->Initialize(config);
  if (!status.ok()) {
    std::cerr << "Graph Init" << status.message() << std::endl;
    throw std::runtime_error("Failed to create the graph");
  }

  mediapipe::StatusOrPoller sop = graph->AddOutputStreamPoller(output_node);

   if (!sop.ok()) {
     std::cerr << "Failed to add poller to graph: [" << sop.status().message() << "]" << std::endl;
     throw std::runtime_error("Failed to add poller to graph");
   }

  assert(sop.ok());

  poller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(sop.value()));

  graph->StartRun({});
}

FaceMeshGraph::~FaceMeshGraph() {
  graph->CloseInputStream(kInputStream);
  graph->WaitUntilDone();
}

bool FaceMeshGraph::process(const cv::Mat *input, Landmark output[478]) {
  auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
      mediapipe::ImageFormat::SRGB, input->cols, input->rows,
      mediapipe::ImageFrame::kDefaultAlignmentBoundary);

  cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
  input->copyTo(input_frame_mat);

  size_t frame_timestamp_us =
      (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;

  absl::Status run_status = graph->AddPacketToInputStream(
      kInputStream, mediapipe::Adopt(input_frame.release())
                        .At(mediapipe::Timestamp(frame_timestamp_us)));

  if (!run_status.ok()) {
    std::cerr << "Add Packet error: [" << run_status.message() << "]" << std::endl;
    return false;
  }

  mediapipe::Packet packet;
  if (poller->QueueSize() > 0) {
    if (poller->Next(&packet)) {
      auto& faces = packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();

      if (faces.size() > 0) {
          const mediapipe::NormalizedLandmarkList &face = faces.at(0);
          
          for (int idx = 0; idx < face.landmark_size(); ++idx) {
            const mediapipe::NormalizedLandmark& landmark = face.landmark(idx);

            output[idx] = {
              .x = landmark.x(),
              .y = landmark.y(),
              .z = landmark.z(),
              .visibility = 0,
              .presence = 0,
            };
          }
      }
    } else {
        std::cerr << "No packet from poller" << std::endl;
        return false;
    }
  }

  return true;
}
