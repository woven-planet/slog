# Slog: Structured Logging
High performance library for emitting structured logging records.

## Entities
Slog is built on a few concepts that provide a powerful and flexible logging experience:
 * *slog* -- high level library with a few macro definitions. Many examples are available in `slog_test.cc`:
   * *SLOG(severity)* -- similar to glog `LOG(severity)` allows to emit text log messages with some additional features that Slog *event* provides, like, `.addTag()`, `<< SLOG_TAG()`, etc. See `SlogEvent` interface for more details;
   * *SLOG_SCOPE(name)* -- a macro creating an object to track a code scope. See `SlogScope` interface for details;
 * *Primitives* -- lowest level structures to represent a structured log record:
   * *record* -- a sructure with common fields, like timestamp, thread_id, *tags*, call_site_id, etc;
   * *tag* -- a key-value pair that could be added to a Slog *record*;
   * *call site* -- a structure representing a code line, a file and a function where the log record was emitted. Slog *record* stores id of call_site to keep it light-weight;
 * *Events* -- a few low-level classes that build *primitives* when Slog records are emitted in the code:
   * *event* -- a class that constructs a Slog *record* and triggers registered Slog *subscribers* in destructor;
   * *scope* -- a class that generates one event in place where it is created and another event when code execution leaves the scope. It is useful to generate pairs of slog records indicating a scope. In post-processing user can match them and compute scope-related metrics;
* *Context* -- set of objects maintaining state of the Slog. They store *call sites*, *subscribers*, and *elapsed timestamp getter*.

## SLOG
Following `SLOG` example demonstrates all SLOG features:
```
  SLOG(INFO).addTag("demo").addTag("year", 2019)
      << "Object type " << SlogTag("object_type", getObjectType())
      << " is detected by camera " << SlogTag("camera_id", getCameraId());
```
It will print a following message to GLOG:
```
I0404 11:52:51.913633  9812 example.cc:51] Object type bicyle is detected by camera cam0
```
But Slog record will be like this:
```
thread_id: 31071
call_site_id: 1
time_elapsed_ns: 871120898722455
tags {
  key: "demo"
}
tags {
  key: "year"
  value_numeric_data: 2019
  value_type: 2
}
tags {
  value_string: "Object type "
  verbosity: 1
  value_type: 1
}
tags {
  key: "object_type"
  value_string: "bicyle"
  verbosity: 1
  value_type: 1
}
tags {
  value_string: " is detected by camera "
  verbosity: 1
  value_type: 1
}
tags {
  key: "camera_id"
  value_string: "cam0"
  verbosity: 1
  value_type: 1
}
```

## SLOG_SCOPE
Another macro provided by Slog is `SLOG_SCOPE`, e.g:
```
  {
    SLOG_SCOPE("scope_do_something");
    doSomething();
  }
```
It will generate two silent Slog records, one when `SLOG_SCOPE` was created and another on scope exit. Pairing these events allows to compute duration of the scope and all events that were emitted inside it.


# Development

## Conventions
Slog is a multi-language library. Its core logic is implemented with C++ and we use bindings to other languages.
When libraries and build targets implemented in multiple languages we have to avoid name collisions and it is important to follow consistent naming conventions to keep code structure easy to read and understastand.

When dealing with a multi-language library please follow this conventions:
* File names of a library `foo` that is natively implemented should match the library name, i.e. `foo.h` or `foo.py`;
* A file name implementing a binding to a target language should have binding suffix, e.g. `foo_pybind.cc`;
* Bazel target names for a library should be defined in the same directory with explicit suffixes - `_cc` for C++, `_py` for Python, e.g. `path/to/foo:foo_cc` and `path/to/foo:foo_py`.

### Example
Assume we have a library `foo` that is implemented in C++ and binded to Python, a pure python library `bar`, a pure C++ library `zar`, and a library `wee` that has multi-language implementation not using bindings.
* Recommended file names:
  * `foo` lib files: `foo.h`, `foo.cc`, `foo_test.cc`, `foo_pybind.cc`, `foo_test.py`;
  * `bar` lib files: `bar.py`, `bar_test.py`;
  * `zar` lib files: `zar.h`, `zar.cc`, `zar_test.cc`;
  * `wee` lib files: `wee.h`, `wee.cc`, `wee_test.cc`, `we.py`, `we_test.py`.
* Recommended bazel targets names:
  * `foo` lib targets: `:foo_cc`, `:foo_cc_test`, `:foo_py`, `:foo_py_test`;
  * `bar` lib targets: `:bar`, `:bar_test`;
  * `zar` lib targets: `:zar`, `:zar_test`;
  * `wee` lib targets: `:wee_cc`, `:wee_cc_test`, `:wee_py`, `:wee_py_test`;

Note, Only `foo` and `wee` libraries should have suffixes in bazel targets names b/c they are multi-language libraries.
Note, test targets are optional, e.g. sometimes there is no value in adding tests for binded libraries.
