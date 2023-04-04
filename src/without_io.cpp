#include <ROOT/RDataFrame.hxx>
#include <ROOT/RLogger.hxx>

#include <cstdlib> // std::abort
#include <random>
#include <string>

struct SimpleRNG {
  std::mt19937 fDist{std::random_device()()};
  std::uniform_real_distribution<> fGen{0., 1.};

  double operator()() { return fGen(fDist); }
};

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <n_threads>\n";
    return 1;
  }

  auto verbosity = ROOT::Experimental::RLogScopedVerbosity(
      ROOT::Detail::RDF::RDFLogChannel(), ROOT::Experimental::ELogLevel::kInfo);

  const unsigned n_threads = std::stoi(argv[1]);

  if (n_threads > 0)
    ROOT::EnableImplicitMT(n_threads);

  const auto n_entries = 100000000ull;

  ROOT::RDataFrame df(n_entries);

  // one rng per processing slot (all with the same seed for simplicity)
  std::vector<SimpleRNG> rnds(std::max(n_threads, 1u));

  auto h = df.DefineSlot("x", [&](unsigned int slot) { return rnds[slot](); })
               .Filter([](double x) { return x > 0.5; }, {"x"})
               .Histo1D<double>(
                   {"h", "h", 100, 0., 1.},
                   "x"); // passing binning explicitly is better for performance

  // event loop runs here
  h.GetValue();

  const double expected = 0.75;
  if (h->GetMean() - expected > 1e-4) {
    std::cerr << "Sanity check failed: histogram mean is " << h->GetMean()
              << " instead of " << expected << '\n';
    std::abort();
  }
}
