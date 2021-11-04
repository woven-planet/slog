import inspect

from src.common.logging.slog.events import events


# Methods starting with `_` are private. Others are public and supposed to
# be used only by external users. To ensure call stack is parsed properly,
# public methods should not call each other, they rather should call only
# private methods.
def _log(severity, message=None, tags=None):
    frame = inspect.stack()[2]
    e = events.SlogEvent(
        severity,
        events.add_or_reuse_call_site_very_slow(
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
    return _log(events.SeverityInfo, message, tags)


def warning(message=None, tags=None):
    return _log(events.SeverityWarning, message, tags)


def error(message=None, tags=None):
    return _log(events.SeverityError, message, tags)


# TODO(vsbus): add mypy annotations. Note: to make it work we need to add
# special file py.typed to the package
# (https://www.python.org/dev/peps/pep-0561/). Maybe it should be
# implemented at pkg_pypi bazel macro level.
def scope(scope_name, tags=None):
    return events.SlogScope(_log(events.SeverityInfo, tags={**(tags or {}), events.kSlogTagKeyScopeName: scope_name}))


def scope_decorator(tags=None):
    def decorator(func):
        def impl(*args, **kwargs):
            scope_name = 'decorated_{}'.format(func.__name__)
            with events.SlogScope(_log(events.SeverityInfo,
                                       tags={**(tags or {}), events.kSlogTagKeyScopeName: scope_name})):
                return func(*args, **kwargs)
        return impl
    return decorator
