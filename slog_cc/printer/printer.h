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

#ifndef slog_cc_printer_printer
#define slog_cc_printer_printer

#include <cstdint>
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
                               const std::string& msg) const;
  std::string stderrLine(const SlogRecord& record,
                         const SlogCallSite& call_site) const;
  void emitStderrLine(const SlogRecord& record, const SlogCallSite& call_site) const;

  std::vector<std::string> tableSplitter() const;
  std::vector<std::string> tableHeader() const;
  std::vector<std::string> tableRow(const SlogRecord& record,
                                    const SlogCallSite& call_site) const;

  std::string flatText(const SlogRecord& record) const;
  std::string slogText(const SlogRecord& record) const;

  std::string debugString(const SlogRecord& record) const;
  std::string debugString(const SlogTag& tag) const;

  std::string jsonString(const SlogRecord& record) const;
  std::string jsonString(const SlogTag& tag) const;

 private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace slog

#endif
