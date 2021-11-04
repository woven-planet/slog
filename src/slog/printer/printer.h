#pragma once

#include <string>
#include <vector>

namespace avsoftware {

class SlogCallSite;
class SlogRecord;
class SlogTag;

class SlogPrinter {
 public:
  static void emitGlog(const SlogRecord& record, const SlogCallSite& call_site);

  static std::vector<std::string> tableSplitter();
  static std::vector<std::string> tableHeader();
  static std::vector<std::string> tableRow(const SlogRecord& record,
                                           const SlogCallSite& call_site);

  static std::string flatText(const SlogRecord& record);
  static std::string slogText(const SlogRecord& record);

  static std::string debugString(const SlogRecord& record);
  static std::string debugString(const SlogTag& tag);

  static std::string jsonString(const SlogRecord& record);
  static std::string jsonString(const SlogTag& tag);
};

}  // namespace avsoftware
