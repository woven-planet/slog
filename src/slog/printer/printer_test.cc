#include "src/slog/printer/printer.h"

#include <glog/logging.h>
#include <gtest/gtest.h>

#include "src/slog/primitives/record.h"

namespace avsoftware {

TEST(SlogPrinterTest, debugStringTag) {
  EXPECT_EQ(R"raw({
  "key": "key",
  "verbosity": "1",
  "valueType": "0",
}
)raw",
            SlogPrinter::debugString(SlogTag("key", SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "0",
  "valueType": "0",
}
)raw",
      SlogPrinter::debugString(SlogTag("key", SlogTagVerbosity::kSilent)));

  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "1",
  "valueType": "1",
  "valueString": "str-value",
}
)raw",
      SlogPrinter::debugString(
          SlogTag("key", "str-value", SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "0",
  "valueType": "1",
  "valueString": "str-value",
}
)raw",
      SlogPrinter::debugString(
          SlogTag("key", "str-value", SlogTagVerbosity::kSilent)));

  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "1",
  "valueType": "2",
  "valueInt": "42",
}
)raw",
      SlogPrinter::debugString(SlogTag("key", 42, SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "0",
  "valueType": "2",
  "valueInt": "73",
}
)raw",
      SlogPrinter::debugString(SlogTag("key", 73, SlogTagVerbosity::kSilent)));

  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "1",
  "valueType": "3",
  "valueDouble": 0.5,
}
)raw",
      SlogPrinter::debugString(SlogTag("key", 0.5, SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({
  "key": "key",
  "verbosity": "0",
  "valueType": "3",
  "valueDouble": 1.5,
}
)raw",
      SlogPrinter::debugString(SlogTag("key", 1.5, SlogTagVerbosity::kSilent)));
}

TEST(SlogPrinterTest, jsonStringTag) {
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "1", "valueType": "0"})raw",
      SlogPrinter::jsonString(SlogTag("key", SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "0", "valueType": "0"})raw",
      SlogPrinter::jsonString(SlogTag("key", SlogTagVerbosity::kSilent)));

  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "1", "valueType": "1", "valueString": "str-value"})raw",
      SlogPrinter::jsonString(
          SlogTag("key", "str-value", SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "0", "valueType": "1", "valueString": "str-value"})raw",
      SlogPrinter::jsonString(
          SlogTag("key", "str-value", SlogTagVerbosity::kSilent)));

  // Note: int64 can't be represented as a number type in JSON, so we represent
  // it as a string.
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "1", "valueType": "2", "valueInt": "42"})raw",
      SlogPrinter::jsonString(SlogTag("key", 42, SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "0", "valueType": "2", "valueInt": "73"})raw",
      SlogPrinter::jsonString(SlogTag("key", 73, SlogTagVerbosity::kSilent)));

  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "1", "valueType": "3", "valueDouble": 0.5})raw",
      SlogPrinter::jsonString(SlogTag("key", 0.5, SlogTagVerbosity::kNoisy)));
  EXPECT_EQ(
      R"raw({"key": "key", "verbosity": "0", "valueType": "3", "valueDouble": 1.5})raw",
      SlogPrinter::jsonString(SlogTag("key", 1.5, SlogTagVerbosity::kSilent)));
}

TEST(SlogPrinterTest, jsonStringRecord) {
  const int32_t kThreadId = 42;
  const int32_t kCallSiteId = 73;
  SlogRecord slog_record(kThreadId, kCallSiteId, google::INFO);
  EXPECT_EQ(
      R"raw({"thread_id": "42", "call_site_id": "73", "severity": "0", "time": {"elapsed_ns": "-1", "global_ns": "-1"}, "tags": []})raw",
      SlogPrinter::jsonString(slog_record));

  slog_record.addTag("just_key");
  slog_record.addTag("string-key", "foo");
  slog_record.addTag("int-key", 123);
  slog_record.addTag("double-key", 12.3);
  EXPECT_EQ(
      R"raw({"thread_id": "42", "call_site_id": "73", "severity": "0", "time": )raw"
      R"raw({"elapsed_ns": "-1", "global_ns": "-1"}, "tags": [{"key": "just_key", )raw"
      R"raw("verbosity": "1", "valueType": "0"}, {"key": "string-key", "verbosity": )raw"
      R"raw("1", "valueType": "1", "valueString": "foo"}, {"key": "int-key", )raw"
      R"raw("verbosity": "1", "valueType": "2", "valueInt": "123"}, {"key": )raw"
      R"raw("double-key", "verbosity": "1", "valueType": "3", "valueDouble": 12.3}]})raw",
      SlogPrinter::jsonString(slog_record));
}

}  // namespace avsoftware
