# RDataFrame benchmarks

A collection of RDataFrame benchmarks.

## The benchmarks

- **without_io**: simple benchmarks with no input files
- [WIP] **rntuple**: rntuple+RDF benchmarks extracted from https://github.com/jblomer/iotools
- [WIP] **adl_benchmarks**: the RDF implementation of [the ADL benchmarks](https://github.com/iris-hep/adl-benchmarks-index), running on 10x input data
- [WIP] **simple_analyses**: simple analysis tasks extracted from [the RDF tutorials](https://root.cern/doc/master/group__tutorial__dataframe.html)
- [WIP] **agc_ttbar**: the RDF implementation of the ttbar analysis task from the [analysis grand challenge](https://github.com/iris-hep/analysis-grand-challenge)

All benchmarks except `without_io` require (possibly large) input files.
They are searched for or, if not found, downloaded into the directory pointed by the environment variable `RDF_BENCHMARKS_DATA_DIR`.

## About code duplication

Most of these benchmarks are duplicating code that lives elsewhere, as mentioned above.

In those cases I cannot guarantee that the version of the code in this repository is up to
date with respect to the upstream implementation, but the source code and/or the git log should indicate to which revision in the upstream repos the versions in this repository correspond to.
