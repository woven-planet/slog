// NOLINT(namespace-avsoftware)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "src/slog/buffer/buffer.h"

PYBIND11_MODULE(buffer, m) {
  pybind11::class_<avsoftware::SlogBufferData>(m, "SlogBufferData")
      .def(pybind11::init<>())
      .def_readwrite("records", &avsoftware::SlogBufferData::records)
      .def_readwrite("call_sites", &avsoftware::SlogBufferData::call_sites);

  pybind11::class_<avsoftware::SlogBuffer>(m, "SlogBuffer")
      .def(pybind11::init<>())
      .def("flush", &avsoftware::SlogBuffer::flush)
      .def("waitSlogQueue", &avsoftware::SlogBuffer::waitSlogQueue)
      .def("__enter__",
           [](avsoftware::SlogBuffer* slog_buffer) { return slog_buffer; })
      .def("__exit__", [](avsoftware::SlogBuffer&, pybind11::object,
                          pybind11::object, pybind11::object) {});
  ;
}
