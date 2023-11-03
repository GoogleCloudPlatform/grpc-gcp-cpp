### How to build and upload it to the registry

This is an example that the gRPC team uses internally, so you may need to change `IMAGE_NAME`` for your needs.

#### Build and upload new docker image

Update the `IMAGE_VERSION`` to match the version of the image you want to use.
Execute this command from the root directory of this repository.

```
export IMAGE_NAME=us-docker.pkg.dev/grpc-testing/testing-images-public/grpc-gcp-cpp-gcs-benchmark
export IMAGE_VERSION=20231102.0
docker build -t $IMAGE_NAME:$IMAGE_VERSION -f e2e-examples/gcs/benchmark_docker/Dockerfile .
docker push $IMAGE_NAME:$IMAGE_VERSION
```

#### Add latest label to the image (optional)

```
docker tag $IMAGE_NAME:$IMAGE_VERSION $IMAGE_NAME:latest
docker push $IMAGE_NAME:latest
```
