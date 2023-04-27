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

#ifndef slog_cc_util_string_util
#define slog_cc_util_string_util

#include <sstream>
#include <string>
#include <vector>

namespace slog {
namespace util {

// Return a string which is the concatenation of the strings in the iterable
// defined by begin, end The separator between elements is "delimiter". f is the
// function used to render the elements
//
// For example join(std::vector<int>({1,2,3})) will return "1, 2, 3"
//
// There are other variants of join below with different interfaces depending on
// the use case. Usage examples: std::vector<std::string> data1; LOG(INFO) <<
// join(data1); LOG(INFO) << join(data1.begin(), data1.end());
//
// std::vector<int> data2;
// LOG(INFO) << join(data2, "\n");
//
// More examples in string_util_test.cc
template <class T>
std::string join(const std::vector<T>& container,
                 const std::string& delimiter) {
  std::stringstream stream;
  bool empty = true;
  for (const auto& item : container) {
    if (empty) {
      empty = false;
    } else {
      stream << delimiter;
    }
    stream << item;
  }
  return stream.str();
}

std::vector<std::string> split(const std::string& s, const char delim,
                               bool remove_empty_tokens = false);

// Like sprintf but we return a std::string instead.
// Note: This function will truncate the result string if it is too large
// (>16K).
std::string stringPrintf(const char* format, ...);

bool startsWith(const std::string& str, const std::string& prefix);

std::string escapeIvalidJsonCharacters(const std::string& str);

}  // namespace util
}  // end namespace slog

#endif
