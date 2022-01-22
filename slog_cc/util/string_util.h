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
// // join with formatting
// LOG(INFO) << join(data2, "\n", [](int a) { stringPrintf("%08x", a);} );
//
// More examples in string_util_test.cc
template <typename Iterator, typename F>
std::string join(const Iterator& begin, const Iterator& end,
                 const std::string& delimiter, const F& f) {
  std::stringstream stream;
  for (Iterator i = begin; i != end; ++i) {
    if (i != begin) stream << delimiter;
    stream << f(*i);
  }
  return stream.str();
}

template <typename Container, typename F>
std::string join(const Container& container, const std::string& delimiter,
                 const F& f) {
  return join(container.begin(), container.end(), delimiter, f);
}

template <typename Iterator>
std::string join(const Iterator& begin, const Iterator& end,
                 const std::string& delimiter = ", ") {
  return join(begin, end, delimiter,
              [](const typename Iterator::value_type& elem) { return elem; });
}

template <typename Container>
std::string join(const Container& container,
                 const std::string& delimiter = ", ") {
  return join(container.begin(), container.end(), delimiter);
}

std::vector<std::string> split(const std::string& s, const char delim,
                               bool remove_empty_tokens = false);

// Like sprintf but we return a std::string instead.
// Note: This function will truncate the result string if it is too large
// (>16K).
std::string stringPrintf(const char* format, ...);

}  // namespace util
}  // end namespace slog

#endif
