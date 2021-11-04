// NOLINT(namespace-avsoftware)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "src/primitives/call_site.h"
#include "src/primitives/record.h"
#include "src/printer/printer.h"

PYBIND11_MODULE(primitives, m) {
  pybind11::class_<avsoftware::SlogRecord>(m, "SlogRecord")
      .def(pybind11::init<int32_t, int32_t, int8_t>())
      .def("__str__", [](const avsoftware::SlogRecord& record) {
        return avsoftware::SlogPrinter::jsonString(record);
      });

  pybind11::class_<avsoftware::SlogCallSite>(m, "SlogCallSite")
      .def(pybind11::init<const std::string&, const std::string&, int32_t>())
      .def("function", &avsoftware::SlogCallSite::function)
      .def("file", &avsoftware::SlogCallSite::file)
      .def("line", &avsoftware::SlogCallSite::line);
}
