# Copyright 2022 Woven Planet Holdings
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import inspect

from slog_py import slog_pybind
from slog_py.slog_pybind import SlogContext

# Methods starting with `_` are private. Others are public and supposed to
# be used only by external users. To ensure call stack is parsed properly,
# public methods should not call each other, they rather should call only
# private methods.


def _log(severity, message=None, tags=None):
    frame = inspect.stack()[2]
    e = slog_pybind.SlogEvent(
        severity,
        slog_pybind.add_or_reuse_call_site_very_slow(
            frame.function,
            frame.filename,
            frame.lineno))
    if message:
        e.emit_value(message)
    if tags:
        for key, value in sorted(tags.items()):
            e.add_tag(key, value)
    return e


def info(message=None, tags=None):
    return _log(slog_pybind.SEVERITY_INFO, message, tags)


def warning(message=None, tags=None):
    return _log(slog_pybind.SEVERITY_WARNING, message, tags)


def error(message=None, tags=None):
    return _log(slog_pybind.SEVERITY_ERROR, message, tags)


def scope(scope_name, tags=None):
    return slog_pybind.SlogScope(_log(slog_pybind.SEVERITY_INFO, tags={**(tags or {}), slog_pybind.SLOG_TAG_KEY_SCOPE_NAME: scope_name}))


def scope_decorator(tags=None):
    def decorator(func):
        def impl(*args, **kwargs):
            scope_name = 'decorated_{}'.format(func.__name__)
            with slog_pybind.SlogScope(_log(slog_pybind.SEVERITY_INFO,
                                            tags={**(tags or {}), slog_pybind.SLOG_TAG_KEY_SCOPE_NAME: scope_name})):
                return func(*args, **kwargs)
        return impl
    return decorator
