Slog Tracing tool allows recording slog events in the format of Chrome Tracing Events so you can visualize them in Chrome.

For usage example see `example_slog_trace` target in the `BUILD` file and implementatin in the `example_slog_trace.cpp`. You can run this example with the following command:
```
bazelisk run slog_cc/analysis_tools/tracing:example_slog_trace
```

It will write slog tracing events to the `/tmp/slog-trace.json` file.
You can open and visualize this file with Chrome browser:
* type `chrome://tracing` in the search bar,
* click "Load" button and select slog-tracing.json file;
You will see a visualization similar to this one:
<img width="1623" alt="Screen Shot 2022-08-23 at 11 00 34 PM" src="https://user-images.githubusercontent.com/5026554/186342136-6d9330e6-d0d6-42cb-8689-29ccbd62038b.png">

