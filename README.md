# RDataFrame benchmarks

A collection of RDataFrame benchmarks.

## The benchmarks

- [X] **without_io**: simple benchmarks with no input files
- [X] **just_reading_arrays**: simple benchmark that reads an array of floats from an input file
- [X] **dimuonanalysis**: simple analysis task from [the RDF tutorials](https://root.cern/doc/master/group__tutorial__dataframe.html); if the file name ends with `.ntuple`, it switches to `RNTuple` as a backend
- [ ] **agc_ttbar**: the RDF implementation of the ttbar analysis task from the [analysis grand challenge](https://github.com/iris-hep/analysis-grand-challenge) -- still to be added

## Examples

### Building the project

```
$ mkdir build
$ cmake -S . -B build -G Ninja -DCMAKE_INSTALL_PREFIX=build/install -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS="-fno-omit-frame-pointer"
$ cmake --build build -- -j16 install
```

### Running the dimuon analysis

```
$ ./build/install/bin/dimuonanalysis 0 ./data/Run2012BC_DoubleMuParked_Muons.root
```

### Creating a zstd-compressed RNTuple input from the TTree input

```
$ ./build/install/bin/tree2ntuple data/Run2012BC_DoubleMuParked_Muons.root data/Run2012BC_DoubleMuParked_Muons.zstd.ntuple Events 505
```

## Input datasets

Several benchmarks require (possibly large) input files. The benchmark's source code has an introduction that indicates where to download the input data from.

## Attribution and code duplication

Most of these benchmarks are duplicating code that lives elsewhere, as mentioned above, and their source code mentions
where they were copied from.

In general I cannot guarantee that the version of the code in this repository is up to
date with respect to the upstream implementation, but the source code and/or the git log should indicate to which revision in the upstream repos the versions in this repository correspond to.
