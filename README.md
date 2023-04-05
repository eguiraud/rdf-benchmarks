# RDataFrame benchmarks

A collection of RDataFrame benchmarks.

## The benchmarks

- [X] **without_io**: simple benchmarks with no input files
- [X] **just_reading_arrays**: simple benchmark that reads an array of floats from an input file
- [X] **dimuonanalysis**: simple analysis task from [the RDF tutorials](https://root.cern/doc/master/group__tutorial__dataframe.html)
- [ ] **rntuple**: rntuple+RDF benchmarks extracted from https://github.com/jblomer/iotools
- [ ] **adl_benchmarks**: the RDF implementation of [the ADL benchmarks](https://github.com/iris-hep/adl-benchmarks-index), running on 10x input data
- [ ] **agc_ttbar**: the RDF implementation of the ttbar analysis task from the [analysis grand challenge](https://github.com/iris-hep/analysis-grand-challenge)

## Input datasets

Several benchmarks require (possibly large) input files.
They are searched for in the directory pointed by the environment variable `RDF_BENCHMARKS_DATA_DIR` or `$PWD/data` as a fallback.

## Attribution and code duplication

Most of these benchmarks are duplicating code that lives elsewhere, as mentioned above, and their source code mentions
where they were copied from.

In general I cannot guarantee that the version of the code in this repository is up to
date with respect to the upstream implementation, but the source code and/or the git log should indicate to which revision in the upstream repos the versions in this repository correspond to.
