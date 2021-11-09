#include "src/primitives/record.h"

#include <sstream>
#include <string>
#include <vector>

#include "src/primitives/tag.h"

namespace avsoftware {

bool SlogRecord::isNoisy() const {
  for (const auto& tag : tags_) {
    if (tag.verbosity() == SlogTagVerbosity::kNoisy) {
      return true;
    }
  }
  return false;
}

}  // namespace avsoftware