#ifndef slog_cc_buffer_buffer_data
#define slog_cc_buffer_buffer_data

#include <memory>
#include <vector>

#include "slog_cc/primitives/call_site.h"
#include "slog_cc/primitives/record.h"

namespace slog {

// Encapsulation of a list of slog records and call sites map at the moment of
// flushing the buffer.
struct SlogBufferData {
  std::vector<SlogRecord> records;
  std::vector<SlogCallSite> call_sites;
};

}  // namespace slog

#endif
