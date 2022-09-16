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
FROM ubuntu:20.04 AS builder

WORKDIR /mediapipe

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -qq update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc-8 g++-8 \
    ca-certificates \
    curl \
    ffmpeg \
    git \
    wget \
    cmake \
    unzip \
    python3-dev \
    python3-opencv \
    python3-pip \
    libopencv-dev \
    libopencv-core-dev \
    libopencv-highgui-dev \
    libopencv-imgproc-dev \
    libopencv-video-dev \
    libopencv-calib3d-dev \
    libopencv-features2d-dev \
    software-properties-common > /dev/null && \
    add-apt-repository -y ppa:openjdk-r/ppa > /dev/null && \
    apt-get -qq update && apt-get install -y openjdk-8-jdk > /dev/null && \
    apt-get install -y mesa-common-dev libegl1-mesa-dev libgles2-mesa-dev > /dev/null && \
    apt-get install -y mesa-utils > /dev/null && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 100 --slave /usr/bin/g++ g++ /usr/bin/g++-8

RUN pip3 install --upgrade setuptools
RUN pip3 install wheel
RUN pip3 install future
RUN pip3 install absl-py numpy opencv-contrib-python protobuf==3.20.1
RUN pip3 install six==1.14.0
RUN pip3 install tensorflow==2.2.0
RUN pip3 install tf_slim

RUN ln -s /usr/bin/python3 /usr/bin/python

RUN mkdir /bazel && \
    wget -q --no-check-certificate -O /bazel/installer.sh "https://github.com/bazelbuild/bazel/releases/download/5.2.0/bazel-5.2.0-installer-linux-x86_64.sh" && \
    wget -q --no-check-certificate -O /bazel/LICENSE.txt "https://raw.githubusercontent.com/bazelbuild/bazel/master/LICENSE" && \
    chmod +x /bazel/installer.sh && \
    /bazel/installer.sh && \
    rm -f /bazel/installer.sh

COPY WORKSPACE /mediapipe/WORKSPACE
COPY setup_opencv.sh /mediapipe/setup_opencv.sh
COPY third_party/opencv_linux.BUILD /mediapipe/third_party/opencv_linux.BUILD

RUN sh setup_opencv.sh

COPY . /mediapipe

RUN bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/lib/mediagraph

# Publish a smaller image without all the build dependencies

# This image is optmized for rust project
# for the repo owner personal use;

# For other use cases: fork, edit and commit
# then wait until github builds it for you

FROM ubuntu:20.04

ENV CARGO_TERM_COLOR=always
ENV DEBIAN_FRONTEND=noninteractive

ENV CARGO_HOME=/root/.cargo
ENV RUSTUP_HOME=/root/.rustup

ENV PATH="/root/.cargo/bin:${PATH}"

# install opencv and other common dependencies
RUN apt-get -qq update && apt-get install -y --no-install-recommends \
    ffmpeg curl libopencv-dev \
    build-essential llvm-dev libclang-dev clang gcc-8 g++-8 \
    software-properties-common > /dev/null && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# install rust
RUN curl https://sh.rustup.rs -sSf | sh -s -- --default-toolchain stable-x86_64-unknown-linux-gnu -y
RUN cargo install bindgen

# install mediagraph
COPY --from=builder /mediapipe/mediapipe/lib/mediagraph/mediagraph.h /usr/include/mediagraph.h
COPY --from=builder /mediapipe/bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so /usr/lib/libmediagraph.so
COPY --from=builder /mediapipe/bazel-bin/mediapipe/lib/mediagraph/libmediagraph.so /usr/lib/x86_64-linux-gnu/libmediagraph.so
