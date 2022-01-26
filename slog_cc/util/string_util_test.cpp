#include "string_util.h"

#include <gtest/gtest.h>

namespace slog {
namespace util {

TEST(StringUtil, join) {
  EXPECT_EQ("1, 2, 3", join(std::vector<int>({1, 2, 3}), ", "));
  std::vector<int> test1({1, 2, 3, 4, 5, 6});
  EXPECT_EQ("1, 2, 3, 4, 5, 6", join(test1, ", "));
  EXPECT_EQ("1.2.3.4.5.6", join(test1, "."));
}

TEST(StringUtil, stringPrintf) {
  EXPECT_EQ(stringPrintf("%d:%d", 123, 345), "123:345");

  // Make sure stringPrintf does not crash on large strings.
  std::vector<char> temp(16 << 20, 'a');
  temp.push_back(0);
  stringPrintf("%s", temp.data());
}

}  // namespace util
}  // namespace slog
