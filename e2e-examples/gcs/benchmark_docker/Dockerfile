# Copyright 2022 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

FROM debian:stable

## Install JDK
RUN apt update && apt -y install curl default-jdk

## Install Bazel
RUN apt-get install -y gnupg2
RUN curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
RUN echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
RUN apt update && apt -y install bazel

# Copy source and build gcs-benchmark
COPY ./ /work
RUN cd /work && bazel build -c opt e2e-examples/gcs/benchmark:benchmark

FROM debian:stable-slim
RUN apt update && apt -y install ca-certificates
COPY --from=0 /work/bazel-bin/e2e-examples/gcs/benchmark/benchmark /
CMD ["./benchmark"]
