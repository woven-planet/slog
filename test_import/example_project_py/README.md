Importing Slog sometimes is not trivial and this directory provides an example how this should be done. There could be challenges with:
* importing transitive dependencies;
* linking C++ targets and Python bindings;
* configuring python toolchain;
* and more...

This provides examples that users can use as a guideline. Also these examples are run in CI for every new release and make an system test. So it helps to catch bugs with linking, visibility and so on...

# Running locally

Go to `test_import/example_project_py` directory:
```
cd test_import/example_project_py
```

To run these test locally a use need to create a proper `WORKSPACE` file based on the `WORKSPACE.template`. It could be done by using `./prepare_workspace.sh` script and providing version to import as an argument. E.g.
```
./prepare_workspace.sh v0.3.80
```

When `WORKSPACE` file is generated you can build and run tests with
```
bazelisk test --test_output=all src:slog_py_test
```
