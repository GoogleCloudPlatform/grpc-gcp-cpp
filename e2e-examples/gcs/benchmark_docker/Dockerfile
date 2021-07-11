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
