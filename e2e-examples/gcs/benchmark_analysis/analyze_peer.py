#!/usr/bin/env python3

import argparse
import csv
import json
import re
import sys
import datetime
from dataclasses import dataclass, field, asdict

def clean_json(s):
  s = re.sub(",[ \t\r\n]+}", "}", s)
  s = re.sub(",[ \t\r\n]+\]", "]", s)
  return s


@dataclass
class PeerData:
  start_time: datetime.datetime = datetime.datetime(1900, 1, 1)
  end_time: datetime.datetime = datetime.datetime(1900, 1, 1)
  read_count: int = 0
  read_bytes: int = 0
  time_map: dict = field(default_factory=dict)

@dataclass
class TimeData:
  read_count: int = 0
  read_bytes: int = 0

def apply_chunks(d, chunks):
  tset = set()
  for c in chunks:
      t = datetime.datetime.fromisoformat(c["time"][:19])
      tset.add(t)
      b = c["bytes"]
      d.read_bytes += b
      d.time_map.setdefault(t, TimeData()).read_bytes += b
  d.read_count += 1
  for t in tset:
      d.time_map[t].read_count += 1

def transform(args):
  peer_map = {}
  with open(args.file) as f:
    j = json.loads(clean_json(f.read()), strict=False)
  for op in j["operations"]:
      status = op["status"] 
      if status != 0:
          continue
      peer = op["peer"]
      d = peer_map.setdefault(peer, PeerData())
      apply_chunks(d, op["chunks"])
  rows = []      
  ts = min(min(v.time_map.keys()) for v in peer_map.values())
  te = max(max(v.time_map.keys()) for v in peer_map.values())
  tc = ts
  while tc <= te:
      row = { "Time": tc.isoformat()}
      total_count = 0
      total_bytes = 0
      for i, p in enumerate(peer_map.values()):
          td = p.time_map.get(tc, None)
          if td:
              row["c" + str(i + 1)] = td.read_count
              row["t" + str(i + 1)] = td.read_bytes
              total_count += td.read_count
              total_bytes += td.read_bytes
      row["c_all"] = total_count
      row["t_all"] = total_bytes
      rows.append(row)
      tc += datetime.timedelta(seconds=1)

  fields = ['Time', 'c_all', 't_all']
  for i in range(len(peer_map)):
    fields.append("c" + str(i + 1))
    fields.append("t" + str(i + 1))
  w = csv.DictWriter(sys.stdout, fields, delimiter='\t')
  w.writeheader()
  w.writerows(rows)


def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("file", help="the path of report file")
  args = parser.parse_args()
  transform(args)


if __name__ == "__main__":
  main()
