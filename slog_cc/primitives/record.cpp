#include "slog_cc/primitives/record.h"

#include <sstream>
#include <string>
#include <vector>

#include "slog_cc/primitives/tag.h"

namespace slog {

bool SlogRecord::isNoisy() const {
  for (const auto& tag : tags_) {
    if (tag.verbosity() == SlogTagVerbosity::kNoisy) {
      return true;
    }
  }
  return false;
}

}  // namespace slog
