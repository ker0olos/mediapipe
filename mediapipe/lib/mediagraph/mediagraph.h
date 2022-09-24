#include <opencv2/core/mat.hpp>

class FaceMeshGraph;
struct Landmark;
struct Landmark {
  float x;
  float y;
  float z;
  float visibility;
  float presence;
};
class FaceMeshGraph {
  public:
    FaceMeshGraph();
    bool process(const cv::Mat *input, Landmark output[478]);
    ~FaceMeshGraph();
  private:
    void* poller;
    void* graph;
};
class PoseGraph;
class PoseGraph {
  public: 
    PoseGraph();
    bool process(const cv::Mat *input, Landmark output[33]);
    ~PoseGraph();
  private: 
    void* poller;
    void* graph;
};