// NOLINT(namespace-avsoftware)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "src/context/context.h"
#include "src/events/event.h"
#include "src/events/scope.h"
#include "src/primitives/tag.h"

PYBIND11_MODULE(events, m) {
  m.attr("SeverityInfo") = google::INFO;
  m.attr("SeverityWarning") = google::WARNING;
  m.attr("SeverityError") = google::ERROR;
  m.attr("SeverityFatal") = google::FATAL;

  m.def("add_or_reuse_call_site_very_slow", [](const std::string& function,
                                               const std::string& file,
                                               const int line) {
    return avsoftware::SlogContext::getInstance().addOrReuseCallSiteVerySlow(
        function, file, line);
  });

  pybind11::class_<avsoftware::SlogEvent>(m, "SlogEvent")
      .def(pybind11::init<const google::LogSeverity, const int32_t>())
      .def("addTag", [](avsoftware::SlogEvent& event, const std::string& key,
                        int64_t value) { event.addTag(key, value); })
      .def("addTag", [](avsoftware::SlogEvent& event, const std::string& key,
                        double value) { event.addTag(key, value); })
      .def("addTag", [](avsoftware::SlogEvent& event, const std::string& key,
                        const std::string& value) { event.addTag(key, value); })
      .def("emitValue", [](avsoftware::SlogEvent& event,
                           const std::string& value) { event << value; });

  m.attr("kSlogTagKeyScopeName") = avsoftware::kSlogTagKeyScopeName;

  pybind11::class_<avsoftware::SlogScope>(m, "SlogScope")
      .def(pybind11::init<avsoftware::SlogEvent&>())
      .def("__enter__", [](avsoftware::SlogScope&) {})
      .def("__exit__", [](avsoftware::SlogScope&, pybind11::object,
                          pybind11::object, pybind11::object) {});
}
