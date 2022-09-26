A fork without anything special expect a script to build a dynamic library that can be used by other libraries and languages, ~~because google hates making things easy~~

### Build

```bash
bazel build --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/lib/mediagraph
```

```bash
sudo cp bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so /usr/lib/libmediagraph.so && sudo cp mediapipe/lib/mediagraph/mediagraph.h /usr/include/mediagraph.h
```

### Using in Rust

First you need to create a bindings. Here's what I use.

```bash
HEADER_FILE="/usr/include/mediagraph.h"

~/.cargo/bin/bindgen \
--opaque-type "std::.*" \
--opaque-type "cv::.*" \
--allowlist-type "FaceMeshGraph" \
--allowlist-var "FaceMeshGraph" \
--allowlist-function "FaceMeshGraph" \
--allowlist-type "PoseGraph" \
--allowlist-var "PoseGraph" \
--allowlist-function "PoseGraph" \
"$HEADER_FILE" \
-o src/bindings.rs \
-- -xc++ -std=c++14 -I/usr/include/opencv4
```

Then you need to add those couple of lines to your `lib.rs`

```rust
#[link(name = "mediagraph")]
extern "C" {}
```

Then it's as easy as this.

```rust
let face_landmarks_bbox = bindings::FaceBox::new();
let face_landmarks = [bindings::Landmark::new(); 478];
let face_graph = unsafe { bindings::FaceMeshGraph::new() };
let images_as_bytes = include_bytes!("../../examples/images/test_normal.jpeg").to_vec();

let mat = opencv::imgcodecs::imdecode(&opencv::types::VectorOfu8::from_iter(images_as_bytes), opencv::imgcodecs::IMREAD_ANYCOLOR);
let mat_raw = input.as_raw() as *const bindings::cv_Mat;

face_graph.process(mat_raw, face_landmarks.as_mut_ptr(), &mut face_landmarks_bbox);

// I said easy, not short.
```

### Github Actions

```yml
# To easliy use the library in CI. add those bits to your workflow
# this will install libmediagraph.so and the mediagraph.h
# in your system so you can use them like your local machine 
runs-on: ubuntu-latest
container: ghcr.io/ker0olos/mediapipe:opencv4
steps:
  ...
```

### Known Issues

Rust drops this error whenever you are not using `opencv::videoio::VideoCapture::new(..)`

`symbol lookup error: /usr/lib/libmediagraph.so: undefined symbol: _ZN2cv12VideoCaptureC1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEi`

I spent a lot of time trying to fix the issue with no avail. So I ended up doing exacly what it wanted and did this

```rust
opencv::videoio::VideoCapture::from_file("/dev/null", 0);
let face_graph = unsafe { bindings::FaceMeshGraph::new() };
```

Which fixed the compile but casues me nightmares each night. If you know how to fix this. Drop a pull request. 

### Credits

> I literally started learning rust because of those repos showed me that I can use mediapipe in it. Without looking though each commit and learning from it, I wouldn't have ever been able to do something like this.
  - [asprecic/mediapipe](https://github.com/asprecic/mediapipe)
    - [angular-rust/mediapipe](https://github.com/angular-rust/mediapipe)  
      - [julesyoungberg/mediapipe](https://github.com/julesyoungberg/mediapipe)  
