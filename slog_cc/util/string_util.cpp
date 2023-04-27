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

#include "string_util.h"

#include <stdarg.h>
#include <cstring>

#include <iostream>

namespace slog {
namespace util {

std::vector<std::string> split(const std::string& s, const char delim,
                               bool remove_empty_tokens) {
  std::vector<std::string> tokens;
  std::stringstream ss;
  ss.str(s);

  std::string item;
  while (!std::getline(ss, item, delim).fail()) {
    // filter out possible two or more spaces in the string s if necessary.
    if (!remove_empty_tokens || !item.empty()) {
      tokens.emplace_back(std::move(item));
    }
  }

  return tokens;
}

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

bool startsWith(const std::string& str, const std::string& prefix) {
  return str.compare(0, prefix.size(), prefix) == 0;
}

std::string escapeIvalidJsonCharacters(const std::string& str) {
  constexpr char reserved_characters[] = {'\b', '\f', '\n', '\r',
                                          '\t', '\"', '\\', 0};
  constexpr char escape_characters[] = {'b', 'f', 'n', 'r', 't', '\"', '\\', 0};
  std::string result;
  for (const int ch : str) {
    const char* pos = strchr(reserved_characters, ch);
    if (pos != nullptr) {
      result += '\\';
      result += escape_characters[pos - reserved_characters];
    } else {
      result += ch;
    }
  }
  return result;
}

}  // namespace util
}  // end namespace slog
