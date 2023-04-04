#include <ROOT/RDataFrame.hxx>
#include <ROOT/RLogger.hxx>

#include <string>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <n_threads>\n";
    return 1;
  }

  auto verbosity = ROOT::Experimental::RLogScopedVerbosity(
      ROOT::Detail::RDF::RDFLogChannel(), ROOT::Experimental::ELogLevel::kInfo);

  const auto n_threads = std::stoi(argv[1]);

  if (n_threads > 0)
    ROOT::EnableImplicitMT(n_threads);

  const auto n_entries = 100000000ull;

  ROOT::RDataFrame df(n_entries);

  auto h = df.DefineSlot("x", [](unsigned int slot) { return slot / 10.; })
               .Filter([](double x) { return x > 0.5; }, {"x"})
               .Histo1D<double>(
                   {"h", "h", 100, 0., 1.},
                   "x"); // passing binning explicitly is better for performance

  // event loop runs here
  h.GetValue();
}
