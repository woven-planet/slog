#include "slog_trace_subscriber.h"

#include "slog_cc/util/string_util.h"
#include "slog_cc/context/subscribers.h"

namespace slog {

SlogTraceSubscriber CreateSlogTraceSubscriber(
    const std::string& slog_trace_json_filepath) {
  auto state = std::make_shared<SlogTraceSubscriberState>();

  state->file.open(slog_trace_json_filepath, std::ios::out | std::ios::trunc);
  state->file << "{\"traceEvents\": [\n";

  auto json_writer_subscriber =
      slog::SlogContext::getInstance()->createAsyncSubscriber(
          [state](const SlogRecord& r) {
            if (state->min_ts_ns == -1) {
              state->min_ts_ns = r.time().global_ns;
            } else {
              state->file << ",\n";
            }

            std::string json_event;

            const SlogTag* scope_id_tag = r.find_tag(".scope_id");
            if (scope_id_tag) {
              const int64_t scope_id = scope_id_tag->valueInt();
              if (r.find_tag(".scope_open")) {
                state->scope_id_to_name[scope_id] =
                    r.find_tag(".scope_name")->valueString();
              }
              json_event = util::stringPrintf(
                  "{\"name\": \"%s\", \"ph\": \"%c\", \"ts\": %lf, \"pid\": "
                  "\"0\"}",
                  state->scope_id_to_name[scope_id].c_str(),
                  r.find_tag(".scope_open") ? 'B' : 'E',
                  (r.time().global_ns - state->min_ts_ns) / 1e3);
            } else {
              const SlogCallSite call_site =
                  SlogContext::getInstance()->getCallSite(r.call_site_id());
              json_event = util::stringPrintf(
                  "{\"name\": \"%s\", \"ph\": \"%c\", \"ts\": %lf, \"pid\": "
                  "\"0\", \"s\": \"g\"}",
                  SlogPrinter().stderrLine(r, call_site).c_str(), 'i',
                  (r.time().global_ns - state->min_ts_ns) / 1e3);
            }
            state->file << "  " << json_event;
          });

  return SlogTraceSubscriber{state, json_writer_subscriber};
}

}  // namespace slog
