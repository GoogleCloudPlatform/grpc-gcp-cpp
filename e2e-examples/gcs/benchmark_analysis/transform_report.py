#!/usr/bin/env python3

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
