#pragma once

#include <memory>
#include <vector>

#include "src/slog/primitives/call_site.h"
#include "src/slog/primitives/record.h"

namespace avsoftware {

// Encapsulation of a list of slog records and call sites map at the moment of
// flushing the buffer.
struct SlogBufferData {
  std::vector<SlogRecord> records;
  std::vector<SlogCallSite> call_sites;
};

}  // namespace avsoftware
