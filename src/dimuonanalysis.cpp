// From
// https://github.com/root-project/root/blob/ecf76dfbe74146a38d8c14c4b16e78738fdc7f0c/tutorials/dataframe/df102_NanoAODDimuonAnalysis.C
// but without jitted code and without the plotting part.
//
// Input data can be downloaded from
// root://eospublic.cern.ch//eos/opendata/cms/derived-data/AOD2NanoAODOutreachTool/Run2012BC_DoubleMuParked_Muons.root

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include <ROOT/RLogger.hxx>

#include <cstdlib> // std::abort
#include <iostream>
#include <string>

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

  const std::string fname =
      argv[2] + std::string("/Run2012BC_DoubleMuParked_Muons.root");
  ROOT::RDataFrame df("Events", fname);

  auto df_2mu = df.Filter([](unsigned nMuon) { return nMuon == 2; }, {"nMuon"});
  auto df_os = df_2mu.Filter(
      [](const ROOT::RVecI &charges) { return charges[0] != charges[1]; },
      {"Muon_charge"});
  auto df_mass = df_os.Define("Dimuon_mass", ROOT::VecOps::InvariantMass<float>,
                              {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass"});
  auto h = df_mass.Histo1D<float>({"Dimuon_mass",
                                   "Dimuon mass;m_{#mu#mu} (GeV);N_{Events}",
                                   30000, 0.25, 300},
                                  "Dimuon_mass");

  h.GetValue(); // event loop runs here

  const double expected = 34.7494;
  if (h->GetMean() - expected > 1e-5) {
    std::cerr << "Sanity check failed: histogram mean is " << h->GetMean()
              << " instead of " << expected << '\n';
    std::abort();
  }
}
