#include <ROOT/RDataFrame.hxx>

#include "benchmark/benchmark.h"

static void NoIo_DefineFilterAndHisto(benchmark::State &state) {
  const auto n_threads = state.range(0);

  if (n_threads > 0)
    ROOT::EnableImplicitMT(n_threads);

  const auto n_entries = 100000000ull;

  ROOT::RDataFrame df(n_entries);

  auto h = df.DefineSlot("x", [](unsigned int slot) { return slot / 10.; })
               .Filter([](double x) { return x > 0.5; }, {"x"})
               .Histo1D<double>(
                   {"h", "h", 100, 0., 1.},
                   "x"); // passing binning explicitly is better for performance

  for (auto _ : state) {
    // event loop runs here
    // N.B. it does not make sense to have more than 1 iteration!
    h.GetValue();
  }
}
BENCHMARK(NoIo_DefineFilterAndHisto);

BENCHMARK_MAIN();
