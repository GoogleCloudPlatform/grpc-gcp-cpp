#!/usr/bin/env python3
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


import argparse
import csv
import sys


def transform(args):
  result = []
  fields = set()
  for tsvfile in args.file:
    cur = {}
    with open(tsvfile) as f:
      reader = csv.DictReader(f, delimiter='\t')
      for row in reader:
        time = row['Time']
        tag = row['Tag']
        value = row['Throughput']
        if tag in cur:
          result.append(cur)
          cur = {}
        cur[tag] = value
        if "Time" not in cur:
          cur["Time"] = time
        fields.add(tag)
    if cur:
      result.append(cur)

  w = csv.DictWriter(sys.stdout, ['Time', ] + list(fields), delimiter='\t')
  w.writeheader()
  w.writerows(result)


def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("file", nargs='+', help="the path of report file")
  args = parser.parse_args()
  transform(args)


if __name__ == "__main__":
  main()
