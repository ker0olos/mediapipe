/* Copyright 2022 The MediaPipe Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <cstdint>
#include <string>
#include <type_traits>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "mediapipe/framework/api2/builder.h"
#include "mediapipe/framework/api2/port.h"
#include "mediapipe/framework/calculator.pb.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/tasks/cc/components/containers/proto/classifications.pb.h"
#include "mediapipe/tasks/cc/components/processors/classification_postprocessing_graph.h"
#include "mediapipe/tasks/cc/components/processors/proto/classification_postprocessing_graph_options.pb.h"
#include "mediapipe/tasks/cc/components/proto/text_preprocessing_graph_options.pb.h"
#include "mediapipe/tasks/cc/components/text_preprocessing_graph.h"
#include "mediapipe/tasks/cc/core/model_resources.h"
#include "mediapipe/tasks/cc/core/model_task_graph.h"
#include "mediapipe/tasks/cc/core/proto/model_resources_calculator.pb.h"
#include "mediapipe/tasks/cc/text/text_classifier/proto/text_classifier_graph_options.pb.h"

namespace mediapipe {
namespace tasks {
namespace text {
namespace text_classifier {

namespace {

using ::mediapipe::api2::Input;
using ::mediapipe::api2::Output;
using ::mediapipe::api2::builder::Graph;
using ::mediapipe::api2::builder::Source;
using ::mediapipe::tasks::components::containers::proto::ClassificationResult;
using ::mediapipe::tasks::core::ModelResources;

constexpr char kClassificationResultTag[] = "CLASSIFICATION_RESULT";
constexpr char kClassificationsTag[] = "CLASSIFICATIONS";
constexpr char kTextTag[] = "TEXT";
constexpr char kMetadataExtractorTag[] = "METADATA_EXTRACTOR";
constexpr char kTensorsTag[] = "TENSORS";

// TODO: remove once Java API migration is over.
// Struct holding the different output streams produced by the text classifier.
struct TextClassifierOutputStreams {
  Source<ClassificationResult> classification_result;
  Source<ClassificationResult> classifications;
};

}  // namespace

// A "TextClassifierGraph" performs Natural Language classification (including
// BERT-based text classification).
// - Accepts input text and outputs classification results on CPU.
//
// Inputs:
//   TEXT - std::string
//     Input text to perform classification on.
//
// Outputs:
//   CLASSIFICATIONS - ClassificationResult @Optional
//     The classification results aggregated by classifier head.
// TODO: remove once Java API migration is over.
//   CLASSIFICATION_RESULT - (DEPRECATED) ClassificationResult @Optional
//     The aggregated classification result object that has 3 dimensions:
//     (classification head, classification timestamp, classification category).
//
// Example:
// node {
//   calculator: "mediapipe.tasks.text.text_classifier.TextClassifierGraph"
//   input_stream: "TEXT:text_in"
//   output_stream: "CLASSIFICATIONS:classifications_out"
//   options {
//     [mediapipe.tasks.text.text_classifier.proto.TextClassifierGraphOptions.ext]
//     {
//       base_options {
//         model_asset {
//           file_name: "/path/to/model.tflite"
//         }
//       }
//     }
//   }
// }
class TextClassifierGraph : public core::ModelTaskGraph {
 public:
  absl::StatusOr<CalculatorGraphConfig> GetConfig(
      SubgraphContext* sc) override {
    ASSIGN_OR_RETURN(
        const ModelResources* model_resources,
        CreateModelResources<proto::TextClassifierGraphOptions>(sc));
    Graph graph;
    ASSIGN_OR_RETURN(
        auto output_streams,
        BuildTextClassifierTask(
            sc->Options<proto::TextClassifierGraphOptions>(), *model_resources,
            graph[Input<std::string>(kTextTag)], graph));
    output_streams.classification_result >>
        graph[Output<ClassificationResult>(kClassificationResultTag)];
    output_streams.classifications >>
        graph[Output<ClassificationResult>(kClassificationsTag)];
    return graph.GetConfig();
  }

 private:
  // Adds a mediapipe TextClassifier task graph into the provided
  // builder::Graph instance. The TextClassifier task takes an input
  // text (std::string) and returns one classification result per output head
  // specified by the model.
  //
  // task_options: the mediapipe tasks TextClassifierGraphOptions proto.
  // model_resources: the ModelResources object initialized from a
  //   TextClassifier model file with model metadata.
  // text_in: (std::string) stream to run text classification on.
  // graph: the mediapipe builder::Graph instance to be updated.
  absl::StatusOr<TextClassifierOutputStreams> BuildTextClassifierTask(
      const proto::TextClassifierGraphOptions& task_options,
      const ModelResources& model_resources, Source<std::string> text_in,
      Graph& graph) {
    // Adds preprocessing calculators and connects them to the text input
    // stream.
    auto& preprocessing =
        graph.AddNode("mediapipe.tasks.components.TextPreprocessingSubgraph");
    MP_RETURN_IF_ERROR(components::ConfigureTextPreprocessingSubgraph(
        model_resources,
        preprocessing.GetOptions<
            tasks::components::proto::TextPreprocessingGraphOptions>()));
    text_in >> preprocessing.In(kTextTag);

    // Adds both InferenceCalculator and ModelResourcesCalculator.
    auto& inference = AddInference(
        model_resources, task_options.base_options().acceleration(), graph);
    // The metadata extractor side-output comes from the
    // ModelResourcesCalculator.
    inference.SideOut(kMetadataExtractorTag) >>
        preprocessing.SideIn(kMetadataExtractorTag);
    preprocessing.Out(kTensorsTag) >> inference.In(kTensorsTag);

    // Adds postprocessing calculators and connects them to the graph output.
    auto& postprocessing = graph.AddNode(
        "mediapipe.tasks.components.processors."
        "ClassificationPostprocessingGraph");
    MP_RETURN_IF_ERROR(
        components::processors::ConfigureClassificationPostprocessingGraph(
            model_resources, task_options.classifier_options(),
            &postprocessing
                 .GetOptions<components::processors::proto::
                                 ClassificationPostprocessingGraphOptions>()));
    inference.Out(kTensorsTag) >> postprocessing.In(kTensorsTag);

    // Outputs the aggregated classification result as the subgraph output
    // stream.
    return TextClassifierOutputStreams{
        /*classification_result=*/postprocessing[Output<ClassificationResult>(
            kClassificationResultTag)],
        /*classifications=*/postprocessing[Output<ClassificationResult>(
            kClassificationsTag)]};
  }
};

REGISTER_MEDIAPIPE_GRAPH(
    ::mediapipe::tasks::text::text_classifier::TextClassifierGraph);

}  // namespace text_classifier
}  // namespace text
}  // namespace tasks
}  // namespace mediapipe
