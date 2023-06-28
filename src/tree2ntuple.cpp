#include <ROOT/RNTupleImporter.hxx>

#include <iostream>
#include <string>

using RNTupleImporter = ROOT::Experimental::RNTupleImporter;

// Usage: ./tree2ntuple <INPUT> <OUTPUT> <TREENAME> <COMPRESSION>
// Example: ./tree2ntuple data/file.root data/file.ntuple 505 Events
int main(int argc, char **argv) {
  if (argc < 5) {
    std::cerr
        << "Usage: ./tree2ntuple <INPUT> <OUTPUT> <TREENAME> <COMPRESSION>\n";
    return 1;
  }

  const std::string input = argv[1];
  const std::string output = argv[2];
  const std::string treeName = argv[3];
  const int compressionSettings = std::stoi(argv[4]);

  auto importer = RNTupleImporter::Create(input, treeName, output);

  auto options = importer->GetWriteOptions();
  options.SetCompression(compressionSettings);
  importer->SetWriteOptions(options);
  importer->Import();
}
