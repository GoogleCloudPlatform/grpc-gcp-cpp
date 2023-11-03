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

## Install GCC
RUN apt update && apt -y install build-essential wget

## Install Bazel
RUN wget https://github.com/bazelbuild/bazelisk/releases/download/v1.18.0/bazelisk-linux-amd64 && \
    chmod +x bazelisk-linux-amd64 && \
    mv bazelisk-linux-amd64 /usr/local/bin/bazel

# Copy source and build gcs-benchmark
COPY ./ /work
RUN cd /work && bazel build -c opt e2e-examples/gcs/benchmark:benchmark

# Creating a new slim docker with the benchmark binary only
FROM debian:stable-slim
RUN apt update && apt -y install ca-certificates
COPY --from=0 /work/bazel-bin/e2e-examples/gcs/benchmark/benchmark /
CMD ["./benchmark"]
