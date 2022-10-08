# Copyright 2019 The MediaPipe Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//mediapipe/lib:__subpackages__"])

cc_binary(
    name = "mediagraph",
    deps = [
        ":mediagraph_"
    ],
    linkshared = 1 ## Very important
)

cc_library(
    name = "mediagraph_",
    srcs = [ "mediagraph.cc" ],
    hdrs = [ "mediagraph.h" ],
    alwayslink = 1,
    visibility = ["//visibility:public"],
    # deps = [
    #     "//mediapipe/framework:calculator_framework",
    #     "//mediapipe/framework/formats:image_frame",
    #     "//mediapipe/framework/formats:image_frame_opencv",
    #     "//mediapipe/framework/port:file_helpers",
    #     "//mediapipe/framework/port:parse_text_proto",
    #     "//mediapipe/framework/port:status",
        
    #     "//mediapipe/graphs/pose_tracking:pose_tracking_cpu_deps",
    #     "//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
    #     "//mediapipe/graphs/face_mesh:desktop_calculators",
    #     "//mediapipe/graphs/face_mesh:desktop_live_calculators",
    # ],
    deps = [
	    "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:image_frame_opencv",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
        
        # "@com_google_absl//absl/flags:parse",

        # Selfie segmentation
        # "//mediapipe/graphs/selfie_segmentation:selfie_segmentation_cpu_deps",

        # Hand tracking
        # "//mediapipe/calculators/util:annotation_overlay_calculator",
        # "//mediapipe/calculators/util:detections_to_render_data_calculator",
        # "//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
        # "//mediapipe/modules/hand_landmark:hand_landmark_tracking_cpu",

        # Face detection
        # "//mediapipe/graphs/face_detection:face_detection_full_range_desktop_live_deps",

        # Face mesh
        "//mediapipe/graphs/face_mesh:desktop_calculators",
        "//mediapipe/graphs/face_mesh:desktop_live_calculators",

        # Hair segmentation
        # "//mediapipe/graphs/hair_segmentation:desktop_calculators",

        # Holistic tracking
        # "//mediapipe/graphs/holistic_tracking:holistic_tracking_cpu_graph_deps",
        # "//mediapipe/modules/holistic_landmark:multi_person_holistic_landmark_cpu",

        # Iris tracking
        # "//mediapipe/graphs/iris_tracking:iris_tracking_cpu_deps",

        # Object detection
        # "//mediapipe/calculators/core:concatenate_vector_calculator",
        # "//mediapipe/calculators/core:flow_limiter_calculator",
        # "//mediapipe/calculators/core:previous_loopback_calculator",
        # "//mediapipe/calculators/core:split_vector_calculator",
        # "//mediapipe/calculators/image:image_transformation_calculator",
        # "//mediapipe/calculators/tflite:ssd_anchors_calculator",
        # "//mediapipe/calculators/tflite:tflite_converter_calculator",
        # "//mediapipe/calculators/tflite:tflite_inference_calculator",
        # "//mediapipe/calculators/tflite:tflite_tensors_to_detections_calculator",
        # "//mediapipe/calculators/util:detection_label_id_to_text_calculator",
        # "//mediapipe/calculators/util:non_max_suppression_calculator",

        # Pose tracking
        "//mediapipe/graphs/pose_tracking:pose_tracking_cpu_deps",
        # "//mediapipe/modules/pose_landmark:multi_pose_landmark_cpu",
	],
)