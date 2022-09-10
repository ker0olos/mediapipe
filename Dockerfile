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
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc-8 g++-8 \
    ca-certificates \
    curl \
    ffmpeg \
    git \
    wget \
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
    software-properties-common && \
    add-apt-repository -y ppa:openjdk-r/ppa && \
    apt-get update && apt-get install -y openjdk-8-jdk && \
    apt-get install -y mesa-common-dev libegl1-mesa-dev libgles2-mesa-dev && \
    apt-get install -y mesa-utils && \
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

ARG BAZEL_VERSION=5.2.0

RUN mkdir /bazel && \
    wget --no-check-certificate -O /bazel/installer.sh "https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/bazel-${BAZEL_VERSION}-installer-linux-x86_64.sh" && \
    wget --no-check-certificate -O /bazel/LICENSE.txt "https://raw.githubusercontent.com/bazelbuild/bazel/master/LICENSE" && \
    chmod +x /bazel/installer.sh && \
    /bazel/installer.sh  && \
    rm -f /bazel/installer.sh

COPY . /mediapipe/

RUN cd /mediapipe && bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/lib/mediagraph

ENTRYPOINT ["/mediapipe/entrypoint.sh"]