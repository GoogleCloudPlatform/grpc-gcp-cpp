### How to build and upload it to gcs.io

```
export IMAGE_NAME=gcr.io/esun-dataflow/gcs-benchmark
export IMAGE_VERSION=0.3
docker build -t $IMAGE_NAME:$IMAGE_VERSION -f e2e-examples/gcs/benchmark_docker/Dockerfile .
docker push $IMAGE_NAME:$IMAGE_VERSION
```
