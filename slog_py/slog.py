import inspect

from slog_py import slog_pybind

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
        e.emitValue(message)
    if tags:
        for key, value in sorted(tags.items()):
            e.addTag(key, value)
    return e


def info(message=None, tags=None):
    return _log(slog_pybind.SeverityInfo, message, tags)


def warning(message=None, tags=None):
    return _log(slog_pybind.SeverityWarning, message, tags)


def error(message=None, tags=None):
    return _log(slog_pybind.SeverityError, message, tags)


def scope(scope_name, tags=None):
    return slog_pybind.SlogScope(_log(slog_pybind.SeverityInfo, tags={**(tags or {}), slog_pybind.kSlogTagKeyScopeName: scope_name}))


def scope_decorator(tags=None):
    def decorator(func):
        def impl(*args, **kwargs):
            scope_name = 'decorated_{}'.format(func.__name__)
            with slog_pybind.SlogScope(_log(slog_pybind.SeverityInfo,
                                            tags={**(tags or {}), slog_pybind.kSlogTagKeyScopeName: scope_name})):
                return func(*args, **kwargs)
        return impl
    return decorator
