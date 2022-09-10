#!/bin/sh

cp /mediapipe/bazel-bin/mediapipe/lib/mediagraph/libmediagraph /usr/lib/libmediagraph.so
cp /mediapipe/mediapipe/lib/mediagraph/mediagraph.h /usr/include/mediagraph.h

cp /mediapipe/bazel-bin/mediapipe/lib/mediagraph/libmediagraph /usr/lib/x86_64-linux-gnu/libmediagraph.so
cp /mediapipe/mediapipe/lib/mediagraph/mediagraph.h /usr/include/x86_64-linux-gnu/mediagraph.h
