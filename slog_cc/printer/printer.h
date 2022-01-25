#ifndef slog_cc_printer_printer
#define slog_cc_printer_printer

#include <memory>
#include <string>
#include <vector>

namespace slog {

class SlogCallSite;
class SlogRecord;
class SlogTag;

class SlogPrinter {
 public:
  SlogPrinter();

  std::string formatStderrLine(uint8_t severity, int month, int day, int hour,
                               int minute, double second, int thread_id,
                               const std::string& file_name, int lineno,
                               const std::string& msg);
  void emitStderrLine(const SlogRecord& record, const SlogCallSite& call_site);

  std::vector<std::string> tableSplitter();
  std::vector<std::string> tableHeader();
  std::vector<std::string> tableRow(const SlogRecord& record,
                                    const SlogCallSite& call_site);

  std::string flatText(const SlogRecord& record);
  std::string slogText(const SlogRecord& record);

  std::string debugString(const SlogRecord& record);
  std::string debugString(const SlogTag& tag);

  std::string jsonString(const SlogRecord& record);
  std::string jsonString(const SlogTag& tag);

 private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace slog

#endif
