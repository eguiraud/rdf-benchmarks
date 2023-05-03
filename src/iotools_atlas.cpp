// From
// https://github.com/jblomer/iotools/blob/9f84bc07708cdfd2a8b704f3ed0ec0a589a35b31/atlas.cxx
// , stripping out everything but the RDF code.
//
// Input data can be downloaded from
// https://cernbox.cern.ch/files/spaces/eos/project/r/root-eos/public/RNTuple/gg_data~zstd.root
#include <Math/Vector4D.h>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RLogger.hxx>

float ComputeInvariantMassRVec(const ROOT::RVecF &pt, const ROOT::RVecF &eta,
                               const ROOT::RVecF &phi, const ROOT::RVecF &e) {
  ROOT::Math::PtEtaPhiEVector p1(pt[0], eta[0], phi[0], e[0]);
  ROOT::Math::PtEtaPhiEVector p2(pt[1], eta[1], phi[1], e[1]);
  return (p1 + p2).mass() / 1000.0;
}

int main(int argc, char **argv) {
  auto verbosity = ROOT::Experimental::RLogScopedVerbosity(
      ROOT::Detail::RDF::RDFLogChannel(), ROOT::Experimental::ELogLevel::kInfo);

  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <n_threads> <input_file>\n";
    return 1;
  }

  const std::string input_path = argv[2];
  ROOT::RDataFrame df("mini", input_path);

  auto df_P = df.Filter([](bool trigP) { return trigP; }, {"trigP"});
  auto df_goodPhotons =
      df_P.Define("goodphotons",
                  [](const ROOT::RVec<bool> &isTightID,
                     const ROOT::RVec<float> &photonPt,
                     const ROOT::RVec<float> &photonEta) {
                    return isTightID && (photonPt > 25000) &&
                           (abs(photonEta) < 2.37) &&
                           ((abs(photonEta) < 1.37) || (abs(photonEta) > 1.52));
                  },
                  {"photon_isTightID", "photon_pt", "photon_eta"})
          .Filter(
              [](const ROOT::RVec<int> &goodphotons) {
                return ROOT::VecOps::Sum(goodphotons) == 2;
              },
              {"goodphotons"});
  auto df_iso =
      df_goodPhotons
          .Filter(
              [](const ROOT::RVec<float> &ptcone30, const ROOT::RVec<float> &pt,
                 const ROOT::RVec<int> &goodphotons) {
                return Sum(ptcone30[goodphotons] / pt[goodphotons] < 0.065) ==
                       2;
              },
              {"photon_ptcone30", "photon_pt", "goodphotons"})
          .Filter(
              [](const ROOT::RVec<float> &etcone20, const ROOT::RVec<float> &pt,
                 const ROOT::RVec<int> &goodphotons) {
                return Sum(etcone20[goodphotons] / pt[goodphotons] < 0.065) ==
                       2;
              },
              {"photon_etcone20", "photon_pt", "goodphotons"});
  auto df_yy = df_iso.Define(
      "m_yy",
      [](const ROOT::RVec<float> &pt, const ROOT::RVec<float> &eta,
         const ROOT::RVec<float> &phi, const ROOT::RVec<float> &E,
         const ROOT::RVec<int> &good) {
        return ComputeInvariantMassRVec(pt[good], eta[good], phi[good],
                                        E[good]);
      },
      {"photon_pt", "photon_eta", "photon_phi", "photon_E", "goodphotons"});
  auto df_window = df_yy.Filter(
      [](const ROOT::RVec<float> &pt, const ROOT::RVec<int> &good, float m_yy) {
        return (pt[good][0] / 1000.0 / m_yy > 0.35) &&
               (pt[good][1] / 1000.0 / m_yy > 0.25) &&
               ((m_yy > 105) && (m_yy < 160));
      },
      {"photon_pt", "goodphotons", "m_yy"});
  auto hData = df_window.Histo1D<float>(
      {"", "Diphoton invariant mass; m_{#gamma#gamma} [GeV];Events", 30, 105,
       160},
      "m_yy");

  hData.GetValue(); // event loop runs here

  const double expected = 125.0326204;
  if (std::abs(hData->GetMean() - expected) > 1e-5) {
    std::cerr << "Sanity check failed: histogram mean is " << hData->GetMean()
              << " instead of " << expected << '\n';
    std::abort();
  }
}
