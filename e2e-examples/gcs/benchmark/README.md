# GCS gRPC client benchmark

# Examples

## Read

```
bazel run //e2e-examples/gcs/benchmark -- \
  --client=grpc \
  --td=true \
  --operation=read \
  --bucket=gcs-grpc-team-dp-test-us-central1 \
  --object_format=read/128MiB/{t}/128MiB.{o} \
  --object_start=0 \
  --object_stop=100 \
  --read_limit=134217728 \
  --runs=100 \
  --threads=1 \
  --verbose
```

## Random-Read

```
bazel run //e2e-examples/gcs/benchmark -- \
 --client=grpc \
 --td=true \
 --operation=random-read \
 --bucket=gcs-grpc-team-dp-test-us-central1 \
 --object=read/4GiB/1/4GiB.1 \
 --chunk_size=131072 \
 --read_limit=4294967296 \
 --runs=100 \
 --threads=1 \
 --verbose
```

## Write

```
bazel run //e2e-examples/gcs/benchmark -- \
 --client=grpc \
 --td=true \
 --operation=write \
 --bucket=gcs-grpc-team-dp-test-us-central1 \
 --object_format=write/test/128MiB/{t}/128MiB.{o} \
 --object_start=0 \
 --write_size=134217728 \
 --runs=10 \
 --threads=1 \
 --verbose
```
