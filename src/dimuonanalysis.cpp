// From
// https://github.com/root-project/root/blob/ecf76dfbe74146a38d8c14c4b16e78738fdc7f0c/tutorials/dataframe/df102_NanoAODDimuonAnalysis.C
// but without jitted code and without the plotting part.
//
// Input data can be downloaded from
// root://eospublic.cern.ch//eos/opendata/cms/derived-data/AOD2NanoAODOutreachTool/Run2012BC_DoubleMuParked_Muons.root

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RNTupleDS.hxx>
#include "ROOT/RVec.hxx"
#include <ROOT/RLogger.hxx>

#include <cstdlib> // std::abort
#include <iostream>
#include <string>

ROOT::RDataFrame MakeRDF(const std::string &fname) {
  if (fname.size() >= 8 && fname.substr(fname.size() - 7) == ".ntuple")
    return ROOT::RDF::Experimental::FromRNTuple("Events", fname);
  else
    return ROOT::RDataFrame("Events", fname);
}

// A custom sinh implementation (faster than the one from glibc)
template <typename T> T SimpleSinh(T x) {
  const auto e = std::exp(x);
  return 0.5f * (e - 1.f / e);
}

// An invariant mass calculation optimized for working on bulk of events.
// Assumes that the inner vectors in the RVec<RVec<float>> that RDF provides
// actually point to a contiguous slab of memory (which is the case for
// branches for which branch->SupportsBulk() returns true).
template <typename T>
void InvMassBulkIgnoreMaskCustomSinH(
    const ROOT::RDF::Experimental::REventMask &eventMask,
    ROOT::RVec<T> &results, const ROOT::RVec<ROOT::RVec<T>> &pts,
    const ROOT::RVec<ROOT::RVec<T>> &etas,
    const ROOT::RVec<ROOT::RVec<T>> &phis,
    const ROOT::RVec<ROOT::RVec<T>> &masses,
    const ROOT::RVec<unsigned int> &sizes) {

  const auto bulkSize = eventMask.Size();
  const auto nElements =
      std::accumulate(sizes.begin(), sizes.begin() + bulkSize, 0ull);

  const float *pt = &pts[0][0];
  const float *eta = &etas[0][0];
  const float *phi = &phis[0][0];
  const float *mass = &masses[0][0];

  std::vector<T> xs(nElements);
  std::vector<T> ys(nElements);
  std::vector<T> zs(nElements);
  std::vector<T> es2(nElements);
  std::vector<T> es(nElements);

  // trigonometric functions are expensive and don't vectorize so we only call
  // them when needed
  std::size_t elementIdx = 0u;
  for (std::size_t i = 0u; i < bulkSize; ++i) {
    const auto size = sizes[i];
    if (eventMask[i]) {
      for (std::size_t j = 0u; j < size; ++j) {
        const auto pt_ = pt[elementIdx + j];
        const auto phi_ = phi[elementIdx + j];
        xs[elementIdx + j] = pt_ * std::cos(phi_);
        ys[elementIdx + j] = pt_ * std::sin(phi_);
        zs[elementIdx + j] = pt_ * SimpleSinh(eta[elementIdx + j]);
      }
    }
    elementIdx += size;
  }

  // looks like the CPU is happier by calculating these for all elements, even
  // if we'll discard many of the results...
  for (std::size_t i = 0; i < nElements; ++i) {
    es2[i] = pt[i] * pt[i] + zs[i] * zs[i] + mass[i] * mass[i];
  }

  for (std::size_t i = 0; i < nElements; ++i) {
    es[i] = std::sqrt(es2[i]);
  }

  elementIdx = 0u;
  for (std::size_t i = 0; i < bulkSize; ++i) {
    T x_sum = 0.;
    T y_sum = 0.;
    T z_sum = 0.;
    T e_sum = 0.;
    const auto size = sizes[i];
    if (eventMask[i]) {
      for (std::size_t j = 0u; j < sizes[i]; ++j) {
        const auto idx = elementIdx + j;
        x_sum += xs[idx];
        y_sum += ys[idx];
        z_sum += zs[idx];
        e_sum += es[idx];
      }
      results[i] = std::sqrt(e_sum * e_sum - x_sum * x_sum - y_sum * y_sum -
                             z_sum * z_sum);
    }
    elementIdx += size;
  }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <n_threads> <input_file> [<use_bulk_api>]\n";
    return 1;
  }

  auto verbosity = ROOT::Experimental::RLogScopedVerbosity(
      ROOT::Detail::RDF::RDFLogChannel(), ROOT::Experimental::ELogLevel::kInfo);

  const auto n_threads = std::stoi(argv[1]);

  if (n_threads > 0)
    ROOT::EnableImplicitMT(n_threads);

  const std::string fname = argv[2];
  auto df = MakeRDF(fname);

  auto df_2mu = df.Filter([](unsigned nMuon) { return nMuon == 2; }, {"nMuon"});
  auto df_os = df_2mu.Filter(
      [](const ROOT::RVecI &charges) { return charges[0] != charges[1]; },
      {"Muon_charge"});
  auto df_mass =
      argc == 4 && bool(std::stoi(argv[3]))
          ? df_os.Define(
                "Dimuon_mass", InvMassBulkIgnoreMaskCustomSinH<float>,
                {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass", "nMuon"})

          : df_os.Define("Dimuon_mass", ROOT::VecOps::InvariantMass<float>,
                         {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass"});
  auto h = df_mass.Histo1D<float>({"Dimuon_mass",
                                   "Dimuon mass;m_{#mu#mu} (GeV);N_{Events}",
                                   30000, 0.25, 300},
                                  "Dimuon_mass");

  h.GetValue(); // event loop runs here

  const double expected = 34.7493785;
  if (std::abs(h->GetMean() - expected) > 1e-5) {
    std::cerr << "Sanity check failed: histogram mean is " << h->GetMean()
              << " instead of " << expected << '\n';
    std::abort();
  }
}
