// Copyright 2022 Woven Planet Holdings
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "slog_cc/printer/printer.h"

#include <gtest/gtest.h>

#include "slog_cc/primitives/record.h"

namespace slog {

TEST(SlogPrinterTest, formatStderrLine) {
  EXPECT_EQ("I0101 00:00:00.000000 42 foo.cc:73] hello",
            SlogPrinter().formatStderrLine(INFO, 1, 1, 0, 0, 0, 42, "foo.cc",
                                           73, "hello"));
  EXPECT_EQ("W1231 23:59:59.990000 65535 foo.cc:73] hi",
            SlogPrinter().formatStderrLine(WARNING, 12, 31, 23, 59, 59.99,
                                           65535, "foo.cc", 73, "hi"));
  EXPECT_EQ("E1231 23:59:09.123457 1 foo.cc:73] hola",
            SlogPrinter().formatStderrLine(ERROR, 12, 31, 23, 59, 9.1234567, 1,
                                           "foo.cc", 73, "hola"));
}

TEST(SlogPrinterTest, debugStringTag) {
  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "1",
  "valueType": "0",
}
)raw",
      SlogPrinter().debugString(SlogTag("key", SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "0",
  "valueType": "0",
}
)raw",
      SlogPrinter().debugString(SlogTag("key", SlogTagVerbosity::kSilent)));

  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "1",
  "valueType": "1",
  "valueString": "str-value",
}
)raw",
      SlogPrinter().debugString(
          SlogTag("key", "str-value", SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "0",
  "valueType": "1",
  "valueString": "str-value",
}
)raw",
      SlogPrinter().debugString(
          SlogTag("key", "str-value", SlogTagVerbosity::kSilent)));

  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "1",
  "valueType": "2",
  "valueInt": "42",
}
)raw",
      SlogPrinter().debugString(SlogTag("key", 42, SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "0",
  "valueType": "2",
  "valueInt": "73",
}
)raw",
      SlogPrinter().debugString(SlogTag("key", 73, SlogTagVerbosity::kSilent)));

  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "1",
  "valueType": "3",
  "valueDouble": 0.5,
}
)raw",
      SlogPrinter().debugString(SlogTag("key", 0.5, SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "0",
  "valueType": "3",
  "valueDouble": 1.5,
}
)raw",
      SlogPrinter().debugString(
          SlogTag("key", 1.5, SlogTagVerbosity::kSilent)));
}

TEST(SlogPrinterTest, jsonStringTag) {
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "1", "valueType": "0"})raw",
      SlogPrinter().jsonString(SlogTag("key", SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "0", "valueType": "0"})raw",
      SlogPrinter().jsonString(SlogTag("key", SlogTagVerbosity::kSilent)));

  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "1", "valueType": "1", "valueString": "str-value"})raw",
      SlogPrinter().jsonString(
          SlogTag("key", "str-value", SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "0", "valueType": "1", "valueString": "str-value"})raw",
      SlogPrinter().jsonString(
          SlogTag("key", "str-value", SlogTagVerbosity::kSilent)));

  // Note: int64 can't be represented as a number type in JSON, so we represent
  // it as a string.
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "1", "valueType": "2", "valueInt": "42"})raw",
      SlogPrinter().jsonString(SlogTag("key", 42, SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "0", "valueType": "2", "valueInt": "73"})raw",
      SlogPrinter().jsonString(SlogTag("key", 73, SlogTagVerbosity::kSilent)));

  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "1", "valueType": "3", "valueDouble": 0.5})raw",
      SlogPrinter().jsonString(SlogTag("key", 0.5, SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "0", "valueType": "3", "valueDouble": 1.5})raw",
      SlogPrinter().jsonString(SlogTag("key", 1.5, SlogTagVerbosity::kSilent)));
}

TEST(SlogPrinterTest, jsonStringRecord) {
  const int32_t kThreadId = 42;
  const int32_t kCallSiteId = 73;
  SlogRecord slog_record(kThreadId, kCallSiteId, INFO);
  EXPECT_EQ(
      R"raw({"thread_id": "42", "call_site_id": "73", "severity": "2", "time": {"elapsed_ns": "-1", "global_ns": "-1"}, "tags": []})raw",
      SlogPrinter().jsonString(slog_record));

  slog_record.addTag("just_key");
  slog_record.addTag("string-key", "foo");
  slog_record.addTag("int-key", 123);
  slog_record.addTag("double-key", 12.3);
  EXPECT_EQ(
      R"raw({"thread_id": "42", "call_site_id": "73", "severity": "2", "time": )raw"
      R"raw({"elapsed_ns": "-1", "global_ns": "-1"}, "tags": [{"key": "just_key", )raw"
      R"raw("verbosity": "1", "valueType": "0"}, {"key": "string-key", "verbosity": )raw"
      R"raw("1", "valueType": "1", "valueString": "foo"}, {"key": "int-key", )raw"
      R"raw("verbosity": "1", "valueType": "2", "valueInt": "123"}, {"key": )raw"
      R"raw("double-key", "verbosity": "1", "valueType": "3", "valueDouble": 12.3}]})raw",
      SlogPrinter().jsonString(slog_record));
}

}  // namespace slog
