# Copyright 2022 Woven Planet Holdings
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
#
# slog_trace tool takes slog.json file in jsonl (JSON Lines) format and outputs an slog-trace.json file that can be visualized using chrome://tracing feature from the Chrome browser. Each line of input slog.json file should follow a specific structure representing slog record and call site. We have a corresponding subscriber implementation in slog_trace_subscriber.h that generates a correct file, see example_slog_trace.cpp as an example.

import json
import os

from slog_py.slog_pybind import SlogRecord, SlogTimestamps, SlogPrinter, SlogCallSite

trace_events = []
scope_names = {}


def find_tag(tags, tag_name):
    for tag in tags:
        if tag["key"] == tag_name:
            return tag


def log_flat_string(record):
    return str(record)


with open('/tmp/slog.json', 'r') as f:
    min_ts_ns = None
    for line in f.readlines():
        slog_record = json.loads(line)

        if min_ts_ns is None:
            min_ts_ns = int(slog_record["time"]["global_ns"])

        tags = slog_record["tags"]

        if find_tag(tags, ".scope_id") is not None:
            scope_id = find_tag(tags, ".scope_id")["valueInt"]

            if not scope_id in scope_names:
                scope_names[scope_id] = find_tag(
                    tags, ".scope_name")["valueString"]

            trace_events.append({
                "name": scope_names[scope_id],
                "ph": 'E' if find_tag(tags, ".scope_open") is None else 'B',
                "ts": (int(slog_record["time"]["global_ns"]) - min_ts_ns) / 1e3,
                "pid": "foo",
            })
        else:
            rec = SlogRecord(int(slog_record["thread_id"]), int(
                slog_record["call_site_id"]), int(slog_record["severity"]))

            # TODO(viktor): add ctor?
            timestamps = SlogTimestamps()
            timestamps.elapsed_ns = int(slog_record["time"]["elapsed_ns"])
            timestamps.global_ns = int(slog_record["time"]["global_ns"])
            # TODO(viktor): bind enum properly. Figure out if this field need to be printed at all; figure out how to handle non-printed values.
            # timestamps.global_clock_type_id = slog_record["time"]["global_clock_type_id"]
            rec.set_time(timestamps)

            call_site = SlogCallSite("", slog_record["call_site"]["file"], int(
                slog_record["call_site"]["line"]))

            for tag in slog_record["tags"]:
                if "valueString" in tag:
                    rec.add_tag(tag["key"], str(tag["valueString"]))
                if "valueInt" in tag:
                    rec.add_tag(tag["key"], int(tag["valueInt"]))

            trace_events.append({
                "name": SlogPrinter().stderr_line(rec, call_site),
                "ph": 'i',
                "ts": (int(slog_record["time"]["global_ns"]) - min_ts_ns) / 1e3,
                "pid": "foo",
                "s": "g",
            })

print(json.dumps({"traceEvents": trace_events}, indent=2))
