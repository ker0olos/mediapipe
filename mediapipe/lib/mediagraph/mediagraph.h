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
struct FaceBox;
struct FaceBox {
  float xmin;
  float xmax;
  float ymin;
  float ymax;
  float zmin;
  float zmax;
};
class FaceMeshGraph {
  public:
    FaceMeshGraph();
    bool process(const cv::Mat *input, Landmark output[478], FaceBox &face_box);
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