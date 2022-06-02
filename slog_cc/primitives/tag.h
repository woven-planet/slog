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

#ifndef slog_cc_primitives_tag
#define slog_cc_primitives_tag

#include <string>
#include <utility>

#include "slog_cc/util/inline_macro.h"

namespace slog {

enum class SlogTagVerbosity { kSilent, kNoisy };

enum class SlogTagValueType { kNone, kString, kInt, kDouble };

class SlogTag {
 public:
  SLOG_INLINE SlogTag(const char* key,
                      SlogTagVerbosity verbosity = SlogTagVerbosity::kNoisy)
      : key_(key), verbosity_(verbosity) {}

  SLOG_INLINE SlogTag(const std::string& key,
                      SlogTagVerbosity verbosity = SlogTagVerbosity::kNoisy)
      : key_(key), verbosity_(verbosity) {}

  SLOG_INLINE SlogTag(std::string&& key,
                      SlogTagVerbosity verbosity = SlogTagVerbosity::kNoisy)
      : key_(std::move(key)), verbosity_(verbosity) {}

  template <class K>
  SLOG_INLINE SlogTag(K&& key, const char* value,
                      SlogTagVerbosity verbosity = SlogTagVerbosity::kNoisy)
      : key_(std::forward<K>(key)),
        value_string_(value),
        verbosity_(verbosity),
        value_type_(SlogTagValueType::kString) {}

  template <class K>
  SLOG_INLINE SlogTag(K&& key, std::string&& value,
                      SlogTagVerbosity verbosity = SlogTagVerbosity::kNoisy)
      : key_(std::forward<K>(key)),
        value_string_(std::move(value)),
        verbosity_(verbosity),
        value_type_(SlogTagValueType::kString) {}

  template <class K>
  SLOG_INLINE SlogTag(K&& key, const std::string& value,
                      SlogTagVerbosity verbosity = SlogTagVerbosity::kNoisy)
      : key_(std::forward<K>(key)),
        value_string_(value),
        verbosity_(verbosity),
        value_type_(SlogTagValueType::kString) {}

  template <class K, class T,
            typename std::enable_if<std::is_floating_point<T>::value>::type* =
                nullptr>
  SLOG_INLINE SlogTag(K&& key, const T value,
                      SlogTagVerbosity verbosity = SlogTagVerbosity::kNoisy)
      : key_(std::forward<K>(key)),
        value_numeric_data_(pack(static_cast<double>(value))),
        verbosity_(verbosity),
        value_type_(SlogTagValueType::kDouble) {}

  template <
      class K, class T,
      typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
  SLOG_INLINE SlogTag(K&& key, const T value,
                      SlogTagVerbosity verbosity = SlogTagVerbosity::kNoisy)
      : key_(std::forward<K>(key)),
        value_numeric_data_(pack(static_cast<int64_t>(value))),
        verbosity_(verbosity),
        value_type_(SlogTagValueType::kInt) {}

  template <class K, class V>
  SLOG_INLINE SlogTag(K&& key, V&& value_string, uint64_t value_numeric_data,
                      SlogTagVerbosity verbosity, SlogTagValueType value_type)
      : key_(std::forward<K>(key)),
        value_string_(std::forward<V>(value_string)),
        value_numeric_data_(value_numeric_data),
        verbosity_(verbosity),
        value_type_(value_type) {}

  SLOG_INLINE const std::string& key() const { return key_; }

  SLOG_INLINE uint64_t valueNumericData() const { return value_numeric_data_; }
  SLOG_INLINE int64_t valueInt() const {
    return *reinterpret_cast<const int64_t*>(&value_numeric_data_);
  }
  SLOG_INLINE double valueDouble() const {
    const double* ptr = reinterpret_cast<const double*>(&value_numeric_data_);
    return *ptr;
  }
  SLOG_INLINE const std::string& valueString() const { return value_string_; }

  SLOG_INLINE SlogTagVerbosity verbosity() const { return verbosity_; }

  SLOG_INLINE SlogTagValueType valueType() const { return value_type_; }

 private:
  static SLOG_INLINE uint64_t pack(int64_t value) { return value; }
  static SLOG_INLINE uint64_t pack(double value) {
    const uint64_t* ptr = reinterpret_cast<uint64_t*>(&value);
    return *ptr;
  }

  std::string key_;
  std::string value_string_;
  // For better performance we could implement own storage with Small String
  // Optimization, aka SSO, trick here using 32 or 64 bytes.
  uint64_t value_numeric_data_ = 0;
  SlogTagVerbosity verbosity_ = SlogTagVerbosity::kSilent;
  SlogTagValueType value_type_ = SlogTagValueType::kNone;
};

}  // namespace slog

#endif
