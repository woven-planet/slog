#include "string_util.h"

#include <stdarg.h>
#include <cstring>

namespace avsoftware {
namespace util {

std::string stringPrintf(const char* format, ...) {
  va_list va_args;
  va_start(va_args, format);

  const size_t buffer_size = 16 << 10;
  char* const buffer = (char*)alloca(buffer_size);
  const int char_count = vsnprintf(buffer, buffer_size, format, va_args);
  if (char_count < 0) {
    return std::string(
               "stringVPrintf failed; see log for errno. Format string: ") +
           format;
  }
  va_end(va_args);
  return buffer;
}

}  // namespace util
}  // end namespace avsoftware
