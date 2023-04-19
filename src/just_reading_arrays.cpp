// A simple benchmark that just reads a single column containing arrays of
// floats. Originally implemented to compare event-by-event I/O vs bulk I/O for
// array branches.
//
// Input data can be downloaded from
// root://eospublic.cern.ch//eos/opendata/cms/derived-data/AOD2NanoAODOutreachTool/Run2012BC_DoubleMuParked_Muons.root
#include "ROOT/RDataFrame.hxx"
#include <ROOT/RLogger.hxx>
#include <ROOT/RNTupleDS.hxx>
#include "ROOT/RVec.hxx"

#include <cstdlib> // std::abort
#include <iostream>
#include <string>

ROOT::RDataFrame MakeRDF(const std::string &fname) {
  if (fname.size() >= 8 && fname.substr(fname.size() - 7) == ".ntuple")
    return ROOT::RDF::Experimental::FromRNTuple("Events", fname);
  else
    return ROOT::RDataFrame("Events", fname);
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <n_threads> <input_file>\n";
    return 1;
  }

  auto verbosity = ROOT::Experimental::RLogScopedVerbosity(
      ROOT::Detail::RDF::RDFLogChannel(), ROOT::Experimental::ELogLevel::kInfo);

  const auto n_threads = std::stoi(argv[1]);

  if (n_threads > 0)
    ROOT::EnableImplicitMT(n_threads);

  const std::string fname = argv[2];
  ROOT::RDataFrame df = MakeRDF(fname);

  auto m = df.Mean<ROOT::RVecF>("Muon_pt");

  m.GetValue(); // event loop runs here

  const float expected = 20.6919785;
  if (std::abs(*m - expected) > 1e-5) {
    std::cerr << "Sanity check failed: muon_pt mean is " << *m
              << " instead of " << expected << '\n';
    std::abort();
  }
}
