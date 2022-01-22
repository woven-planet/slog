#ifndef slog_cc_printer_printer
#define slog_cc_printer_printer

#include <string>
#include <vector>

namespace slog {

class SlogCallSite;
class SlogRecord;
class SlogTag;

class SlogPrinter {
 public:
  static std::string formatStderrLine(uint8_t severity, int month, int day,
                                      int hour, int minute, double second,
                                      int thread_id,
                                      const std::string& file_name, int lineno,
                                      const std::string& msg);
  static void emitStderrLine(const SlogRecord& record,
                             const SlogCallSite& call_site);

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

}  // namespace slog

#endif
