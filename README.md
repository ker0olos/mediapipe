A fork without anything special expect a script to build a dynamic library that can be used by other libraries and languages, ~~because google hates making things easy~~

### Build

```bash
bazel build --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/lib/mediagraph
```

```bash
sudo cp bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so /usr/lib/libmediagraph.so && sudo cp mediapipe/lib/mediagraph/mediagraph.h /usr/include/mediagraph.h
```

outputs: ```bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so```, header is located at: ```mediapipe/lib/mediagraph/mediagraph.h```


### Credits

> I literally started learning rust because of those repos showed me that I can use mediapipe in it. Without looking though each commit and learning from it, I wouldn't have ever been able to do something like this.
  - [asprecic/mediapipe](https://github.com/asprecic/mediapipe)
    - [angular-rust/mediapipe](https://github.com/angular-rust/mediapipe)  
      - [julesyoungberg/mediapipe](https://github.com/julesyoungberg/mediapipe)  