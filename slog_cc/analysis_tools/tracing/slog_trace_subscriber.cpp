#include "slog_trace_subscriber.h"

#include "slog_cc/context/subscribers.h"
#include "slog_cc/util/string_util.h"

namespace slog {

SlogTraceSubscriber CreateSlogTraceSubscriber(
    const std::string& slog_trace_json_filepath, const SlogTraceConfig config) {
  auto state = std::make_shared<SlogTraceSubscriberState>();

  state->file.open(slog_trace_json_filepath, std::ios::out | std::ios::trunc);
  state->file << "{\"traceEvents\": [\n";

  auto json_writer_subscriber =
      slog::SlogContext::getInstance()->createAsyncSubscriber([state, config](
                                                                  const SlogRecord&
                                                                      r) {
        const SlogTag* scope_id_tag = r.find_tag(".scope_id");

        if (config == SlogTraceConfig::kTrackScopesOnly &&
            scope_id_tag == nullptr) {
          return;
        }

        if (state->min_ts_ns == -1) {
          state->min_ts_ns = r.time().global_ns;
        } else {
          state->file << ",\n";
        }

        std::vector<std::string> str_tags;
        for (const SlogTag& tag : r.tags()) {
          if (tag.key().empty()) {
            // Skip tags with empty key.
            continue;
          }
          if (util::startsWith(tag.key(), ".scope")) {
            // Hide scope internal tags.
            continue;
          }
          str_tags.push_back([&tag]() -> std::string {
            switch (tag.valueType()) {
              case SlogTagValueType::kString:
                return util::stringPrintf(R"raw("%s": "%s")raw",
                                          tag.key().c_str(),
                                          tag.valueString().c_str());
              case SlogTagValueType::kDouble:
                return util::stringPrintf(R"raw("%s": %lf)raw",
                                          tag.key().c_str(), tag.valueDouble());
              case SlogTagValueType::kInt:
                return util::stringPrintf(R"raw("%s": "%d")raw",
                                          tag.key().c_str(), tag.valueInt());
              case SlogTagValueType::kNone:
                return util::stringPrintf(R"raw("%s": "")raw",
                                          tag.key().c_str());
            }
          }());
        }

        std::string json_event;
        if (scope_id_tag) {
          const int64_t scope_id = scope_id_tag->valueInt();
          const bool is_open = r.find_tag(".scope_open");
          if (is_open) {
            state->scope_id_to_name[{r.thread_id(), scope_id}] =
                r.find_tag(".scope_name")->valueString();
          }
          const std::string str_args = [&str_tags]() -> std::string {
            if (str_tags.empty()) {
              return "";
            } else {
              return util::stringPrintf(R"raw("tags": {%s})raw",
                                        util::join(str_tags, ", ").c_str());
            }
          }();
          json_event = util::stringPrintf(
              R"raw({"name": "%s", "ph": "%c", "ts": %lf, "pid": "0", "tid": "%d", "cat": "scope", "args": {%s}})raw",
              state->scope_id_to_name[{r.thread_id(), scope_id}].c_str(),
              is_open ? 'B' : 'E',
              (r.time().global_ns - state->min_ts_ns) / 1e3, r.thread_id(),
              str_args.c_str());
        } else {
          const std::string severity = [&r]() -> std::string {
            switch (r.severity()) {
              case UNKNOWN:
                return "Unknown";
              case DEBUG:
                return "Debug";
              case INFO:
                return "Info";
              case WARNING:
                return "Warning";
              case ERROR:
                return "Error";
              case FATAL:
                return "Fatal";
              default:
                return std::to_string(r.severity());
            }
          }();
          const SlogCallSite call_site =
              SlogContext::getInstance()->getCallSite(r.call_site_id());
          json_event = util::stringPrintf(
              R"raw({"name": "%s", "ph": "%c", "ts": %lf, "pid": "0", "tid": "%d", "s": "t", "cat": "%s", "args": {"log_msg": "%s", "tags": {%s}}})raw",
              severity.c_str(), 'i',
              (r.time().global_ns - state->min_ts_ns) / 1e3, r.thread_id(),
              severity.c_str(), SlogPrinter().stderrLine(r, call_site).c_str(),
              util::join(str_tags, ", ").c_str());
        }
        state->file << "  " << json_event;
      });

  return SlogTraceSubscriber{state, json_writer_subscriber};
}

}  // namespace slog
