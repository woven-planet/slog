#include "src/slog/printer/printer.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include <glog/logging.h>

#include "src/slog/primitives/call_site.h"
#include "src/slog/primitives/record.h"
#include "src/slog/util/string_util.h"

namespace avsoftware {

namespace detail {

enum class ValueStyle { kUnquoted, kQuoted };

template <class T>
std::string keyValueString(const std::string& key, const T& value,
                           ValueStyle value_style = ValueStyle::kQuoted) {
  std::stringstream out;
  out << "\"" << key << "\": ";
  switch (value_style) {
    case ValueStyle::kQuoted:
      out << "\"" << value << "\"";
      break;
    case ValueStyle::kUnquoted:
      out << value;
      break;
  }
  return out.str();
}

std::vector<std::string> keyValueStrings(const SlogTag& tag) {
  std::vector<std::string> kv_strings;
  kv_strings.push_back(detail::keyValueString("key", tag.key()));
  kv_strings.push_back(
      detail::keyValueString("verbosity", (int)tag.verbosity()));
  kv_strings.push_back(
      detail::keyValueString("valueType", (int)tag.valueType()));
  switch (tag.valueType()) {
    case SlogTagValueType::kNone:
      break;
    case SlogTagValueType::kString:
      kv_strings.push_back(
          detail::keyValueString("valueString", tag.valueString()));
      break;
    case SlogTagValueType::kInt:
      kv_strings.push_back(detail::keyValueString("valueInt", tag.valueInt()));
      break;
    case SlogTagValueType::kDouble:
      kv_strings.push_back(detail::keyValueString(
          "valueDouble", tag.valueDouble(), detail::ValueStyle::kUnquoted));
      break;
  };
  return kv_strings;
}

}  // namespace detail

std::string SlogPrinter::debugString(const SlogTag& tag) {
  return "{\n  " + util::join(detail::keyValueStrings(tag), ",\n  ") + ",\n}\n";
}

std::string SlogPrinter::jsonString(const SlogTag& tag) {
  return "{" + util::join(detail::keyValueStrings(tag), ", ") + "}";
}

std::string SlogPrinter::debugString(const SlogRecord& record) {
  std::stringstream s;
  s << "\nthread_id_: " << record.thread_id();
  s << "\ncall_site_id_: " << record.call_site_id();
  s << "\ntime_.elapsed_ns: " << record.time().elapsed_ns;
  s << "\ntime_.global_ns: " << record.time().global_ns;
  s << "\ntags_: ...";
  for (const auto& tag : record.tags()) {
    s << "\n" << debugString(tag);
  }
  return s.str();
}

std::string SlogPrinter::jsonString(const SlogRecord& record) {
  std::vector<std::string> kv_strings;
  kv_strings.push_back(detail::keyValueString("thread_id", record.thread_id()));
  kv_strings.push_back(
      detail::keyValueString("call_site_id", record.call_site_id()));
  kv_strings.push_back(
      detail::keyValueString("severity", (int)record.severity()));
  kv_strings.push_back(detail::keyValueString(
      "time",
      "{" +
          util::join(
              std::vector<std::string>{
                  detail::keyValueString("elapsed_ns",
                                         record.time().elapsed_ns),
                  detail::keyValueString("global_ns", record.time().global_ns),
              },
              ", ") +
          "}",
      detail::ValueStyle::kUnquoted));

  std::vector<std::string> tag_strings;
  for (const auto& tag : record.tags()) {
    tag_strings.push_back(jsonString(tag));
  }
  kv_strings.push_back(
      detail::keyValueString("tags", "[" + util::join(tag_strings, ", ") + "]",
                             detail::ValueStyle::kUnquoted));

  return "{" + util::join(kv_strings, ", ") + "}";
}

std::string SlogPrinter::slogText(const SlogRecord& record) {
  std::string res;
  for (const auto& tag : record.tags()) {
    if (tag.verbosity() == SlogTagVerbosity::kSilent) {
      continue;
    }
    if (tag.key().empty()) {
      res += tag.valueString();
    } else {
      res += "<" + tag.key() + ">";
    }
  }
  return res;
}

std::string SlogPrinter::flatText(const SlogRecord& record) {
  std::string res;
  for (const auto& tag : record.tags()) {
    if (tag.verbosity() == SlogTagVerbosity::kSilent) {
      continue;
    }
    switch (tag.valueType()) {
      case SlogTagValueType::kNone:
        break;
      case SlogTagValueType::kString:
        res += tag.valueString();
        break;
      case SlogTagValueType::kInt:
        res += std::to_string(tag.valueInt());
        break;
      case SlogTagValueType::kDouble:
        res += std::to_string(tag.valueDouble());
        break;
    };
  }
  return res;
}

void SlogPrinter::emitGlog(const SlogRecord& r, const SlogCallSite& cs) {
  // TODO(vsbus): impl and add tests
  // const char severity = r.severity();
  // const std::string file_name = util::split(cs.file(), "/").back();
  // const int lineno = cs.line();
  // const std::string msg = flatText(r);
  // // TODO(vsbus): add test for formatting.
  // util::stringPrintf(
  //   "%c%02d%02d %02d:%02d:%02.6lf %d %s:%d] %s",
  //   severity,
  //   month,
  //   day,
  //   hour,
  //   minute,
  //   second,
  //   thread_id,
  //   file_name.c_str(),
  //   lineno,
  //   msg.c_str()
  // )
  // F1104 18:41:54.512336 25869 lyftbag_util.cc:93] Check failed:
  // !FLAGS_files.empty() --files must be specified
  google::LogMessage(cs.file().c_str(), cs.line(), r.severity()).stream()
      << flatText(r);
}

using Line = std::vector<std::string>;
using Row = std::vector<Line>;
using CellValue = std::vector<std::string>;

struct TableField {
  std::string name;
  size_t width;
  std::function<CellValue(const SlogRecord&, const SlogCallSite&)> func;
};

std::vector<TableField> kTableSchema = {
    {"level", 7,
     [](const SlogRecord& r, const SlogCallSite&) {
       switch (r.severity()) {
         case google::INFO:
           return std::vector<std::string>{"INFO"};
         case google::WARNING:
           return std::vector<std::string>{"WARNING"};
         case google::ERROR:
           return std::vector<std::string>{"ERROR"};
         case google::FATAL:
           return std::vector<std::string>{"FATAL"};
         default:
           return std::vector<std::string>{std::to_string(r.severity())};
       }
     }},
    {"thread", 6,
     [](const SlogRecord& r, const SlogCallSite&) {
       return std::vector<std::string>{std::to_string(r.thread_id())};
     }},
    {"global_time_sec", 20,
     [](const SlogRecord& r, const SlogCallSite&) {
       return std::vector<std::string>{
           std::to_string(r.time().global_ns / 1000000000) + "." +
           std::to_string(r.time().global_ns % 1000000000)};
     }},
    {"file", 32,
     [](const SlogRecord& r, const SlogCallSite& call_site) {
       return std::vector<std::string>{call_site.file()};
     }},
    {"line", 4,
     [](const SlogRecord& r, const SlogCallSite& call_site) {
       return std::vector<std::string>{std::to_string(call_site.line())};
     }},
    {"tag.key", 24,
     [](const SlogRecord& r, const SlogCallSite&) {
       std::vector<std::string> res;
       for (const auto& tag : r.tags()) {
         res.push_back(tag.key());
       }
       return res;
     }},
    {"tag.value", 47,
     [](const SlogRecord& r, const SlogCallSite&) {
       std::vector<std::string> res;
       for (const auto& tag : r.tags()) {
         std::string value;
         if (tag.valueType() == SlogTagValueType::kInt) {
           value = std::to_string(tag.valueInt());
         } else if (tag.valueType() == SlogTagValueType::kDouble) {
           value = std::to_string(tag.valueDouble());
         } else {
           value = tag.valueString();
           std::replace(value.begin(), value.end(), '\n', ' ');
         }
         res.push_back(value);
       }
       return res;
     }},
};

std::string renderLine(const Line& line) {
  CHECK_EQ(line.size(), kTableSchema.size());
  std::vector<std::string> rendered_line;
  for (size_t i = 0; i < line.size(); ++i) {
    rendered_line.emplace_back(
        line[i].size() <= kTableSchema[i].width
            ? std::string(kTableSchema[i].width - line[i].size(), ' ') + line[i]
            : "..." +
                  line[i].substr(line[i].size() - kTableSchema[i].width + 3));
  }

  std::string res = "|";
  for (const std::string& s : rendered_line) {
    res += " " + s + " |";
  }
  return res;
};

std::vector<std::string> SlogPrinter::tableSplitter() {
  Line splitter;
  for (const auto& field : kTableSchema) {
    splitter.emplace_back(std::string(field.width, '-'));
  }
  return {renderLine(splitter)};
}

std::vector<std::string> SlogPrinter::tableHeader() {
  Line header;
  for (const auto& field : kTableSchema) {
    header.emplace_back(field.name);
  }
  return {renderLine(header)};
}

std::vector<std::string> SlogPrinter::tableRow(const SlogRecord& record,
                                               const SlogCallSite& call_site) {
  size_t num_lines = 0;
  for (const auto& field : kTableSchema) {
    num_lines = std::max(num_lines, field.func(record, call_site).size());
  }

  Row row(num_lines);
  for (const auto& field : kTableSchema) {
    CellValue cell_value = field.func(record, call_site);
    while (cell_value.size() < num_lines) {
      cell_value.emplace_back();
    }

    for (size_t i = 0; i < num_lines; ++i) {
      row[i].emplace_back(cell_value[i]);
    }
  }

  std::vector<std::string> res;
  for (const Line& line : row) {
    res.emplace_back(renderLine(line));
  }
  return res;
}

}  // namespace avsoftware
