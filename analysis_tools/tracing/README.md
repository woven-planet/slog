```
bazelisk run analysis_tools/tracing:example_slog_trace
```

It will write slog tracing events to the `/tmp/slog-trace.json` file. You can open and visualize this file with Chrome browser: type `chrome://tracing` in the search bar, click "Open" button and select slog-tracing.json file.
