// NOLINT(namespace-avsoftware)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "slog_cc/buffer/buffer.h"
#include "slog_cc/context/context.h"
#include "slog_cc/events/event.h"
#include "slog_cc/events/scope.h"
#include "slog_cc/primitives/call_site.h"
#include "slog_cc/primitives/record.h"
#include "slog_cc/primitives/tag.h"
#include "slog_cc/printer/printer.h"

PYBIND11_MODULE(slog_pybind, m) {
  pybind11::class_<slog::SlogBufferData>(m, "SlogBufferData")
      .def(pybind11::init<>())
      .def_readwrite("records", &slog::SlogBufferData::records)
      .def_readwrite("call_sites", &slog::SlogBufferData::call_sites);

  pybind11::class_<slog::SlogBuffer>(m, "SlogBuffer")
      .def(pybind11::init<std::shared_ptr<slog::SlogContext>>())
      .def("flush", &slog::SlogBuffer::flush)
      .def("wait_slog_queue", &slog::SlogBuffer::waitSlogQueue)
      .def("__enter__",
           [](slog::SlogBuffer* slog_buffer) { return slog_buffer; })
      .def("__exit__", [](slog::SlogBuffer&, pybind11::object, pybind11::object,
                          pybind11::object) {});
  ;

  m.attr("SEVERITY_INFO") = slog::INFO;
  m.attr("SEVERITY_WARNING") = slog::WARNING;
  m.attr("SEVERITY_ERROR") = slog::ERROR;
  m.attr("SEVERITY_FATAL") = slog::FATAL;

  m.def(
      "add_or_reuse_call_site_very_slow",
      [](const std::string& function, const std::string& file, const int line) {
        return slog::SlogContext::getInstance()->addOrReuseCallSiteVerySlow(
            function, file, line);
      });

  pybind11::class_<slog::SlogContext, std::shared_ptr<slog::SlogContext>>(
      m, "SlogContext")
      .def_static("get_instance", &slog::SlogContext::getInstance);

  pybind11::class_<slog::SlogEvent>(m, "SlogEvent")
      .def(pybind11::init<const int8_t, const int32_t>())
      .def("add_tag", [](slog::SlogEvent& event, const std::string& key,
                         int64_t value) { event.addTag(key, value); })
      .def("add_tag", [](slog::SlogEvent& event, const std::string& key,
                         double value) { event.addTag(key, value); })
      .def("add_tag",
           [](slog::SlogEvent& event, const std::string& key,
              const std::string& value) { event.addTag(key, value); })
      .def("emit_value", [](slog::SlogEvent& event, const std::string& value) {
        event << value;
      });

  m.attr("SLOG_TAG_KEY_SCOPE_NAME") = slog::kSlogTagKeyScopeName;

  pybind11::class_<slog::SlogScope>(m, "SlogScope")
      .def(pybind11::init<slog::SlogEvent&>())
      .def("__enter__", [](slog::SlogScope&) {})
      .def("__exit__", [](slog::SlogScope&, pybind11::object, pybind11::object,
                          pybind11::object) {});

  pybind11::class_<slog::SlogRecord>(m, "SlogRecord")
      .def(pybind11::init<int32_t, int32_t, int8_t>())
      .def("__str__", [](const slog::SlogRecord& record) {
        return slog::SlogPrinter::jsonString(record);
      });

  pybind11::class_<slog::SlogCallSite>(m, "SlogCallSite")
      .def(pybind11::init<const std::string&, const std::string&, int32_t>())
      .def("function", &slog::SlogCallSite::function)
      .def("file", &slog::SlogCallSite::file)
      .def("line", &slog::SlogCallSite::line);
}
