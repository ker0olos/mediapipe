```bash
bazel build --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/lib/mediagraph
```

```bash
sudo cp bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so /usr/lib/libmediagraph.so
```
