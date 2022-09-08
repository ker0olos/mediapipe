#include <opencv2/core/mat.hpp>

class FaceMeshGraph;
struct FaceMesh;
struct Landmark;
struct Landmark {
  float x;
  float y;
  float z;
  float visibility;
  float presence;
};
struct FaceMesh {
    Landmark data[478];
};
class FaceMeshGraph {
  public:
    FaceMeshGraph(const char* graph_config, const char* output_node);
    bool process(const cv::Mat *input, FaceMesh &mesh);
    ~FaceMeshGraph();
  private:
    void* poller;
    void* graph;
};
class PoseGraph;
struct Pose;
struct Pose {
    Landmark data[33];
};
class PoseGraph {
  public: 
    PoseGraph(const char* graph_config, const char* output_node);
    bool process(const cv::Mat *input, Pose &output);
    ~PoseGraph();
  private: 
    void* poller;
    void* graph;
};