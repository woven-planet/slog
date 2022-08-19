Importing Slog sometimes is not trivial and this directory provides an example how this should be done. There could be challenges with:
* importing transitive dependencies;
* linking C++ targets;
* and more...

This provides examples that users can use as a guideline. Also these examples are run in CI for every new release and make a system test. So it helps to catch bugs with linking, visibility and so on...

# Running locally

Go to `test_import/example_project_cc_via_bazel` directory:
```
cd test_import/example_project_cc_via_bazel
```

To run these test locally a user needs to create a proper `WORKSPACE` file based on the `WORKSPACE.template`. It could be done by using `./prepare_workspace.sh` script and providing a `git_commit` as an argument. E.g.
```
./prepare_workspace.sh 2afe53c1032fe0d8878dbe15828c67e76d940d1b
```

When `WORKSPACE` file is generated you can build and run tests with
```
bazelisk test --test_output=all src:slog_cc_test
```
