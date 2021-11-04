#include "string_util.h"

#include <gtest/gtest.h>

namespace avsoftware {
namespace util {

TEST(StringUtil, join) {
  EXPECT_EQ("1, 2, 3", join(std::vector<int>({1, 2, 3})));
  std::vector<int> test1({1, 2, 3, 4, 5, 6});
  EXPECT_EQ("1, 2, 3, 4, 5, 6", join(test1));
  EXPECT_EQ("1, 2, 3, 4, 5, 6", join(test1.begin(), test1.end()));
  EXPECT_EQ("6, 5, 4, 3, 2, 1", join(test1.rbegin(), test1.rend()));
  EXPECT_EQ("1.2.3.4.5.6", join(test1.begin(), test1.end(), "."));
  EXPECT_EQ("1.2.3.4.5.6", join(test1, "."));
  EXPECT_EQ("001 002 003 004 005 006",
            join(test1.begin(), test1.end(), " ",
                 [](int a) { return stringPrintf("%03d", a); }));
  EXPECT_EQ("001 002 003 004 005 006",
            join(test1, " ", [](int a) { return stringPrintf("%03d", a); }));

  std::map<std::string, int> test2({{"a", 12}, {"b", 23}, {"c", 34}});
  EXPECT_EQ(
      "a:12 b:23 c:34",
      join(test2, " ", [](const std::map<std::string, int>::value_type& p) {
        return stringPrintf("%s:%d", p.first.c_str(), p.second);
      }));
}

TEST(StringUtil, stringPrintf) {
  EXPECT_EQ(stringPrintf("%d:%d", 123, 345), "123:345");

  // Make sure stringPrintf does not crash on large strings.
  std::vector<char> temp(16 << 20, 'a');
  temp.push_back(0);
  stringPrintf("%s", temp.data());
}

}  // namespace util
}  // namespace avsoftware
