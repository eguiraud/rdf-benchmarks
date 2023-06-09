message(STATUS "Looking for ROOT")
find_package(ROOT REQUIRED COMPONENTS ROOTDataFrame)
message(STATUS "ROOT ${ROOT_VERSION} found at ${ROOT_BINDIR}")

add_executable(without_io without_io.cpp)
target_link_libraries(without_io PRIVATE ROOT::ROOTDataFrame)
install(TARGETS without_io RUNTIME)

add_executable(dimuonanalysis dimuonanalysis.cpp)
target_link_libraries(dimuonanalysis PRIVATE ROOT::ROOTDataFrame)
install(TARGETS dimuonanalysis RUNTIME)

add_executable(just_reading_arrays just_reading_arrays.cpp)
target_link_libraries(just_reading_arrays PRIVATE ROOT::ROOTDataFrame)
install(TARGETS just_reading_arrays RUNTIME)

add_executable(just_reading_scalars just_reading_scalars.cpp)
target_link_libraries(just_reading_scalars PRIVATE ROOT::ROOTDataFrame)
install(TARGETS just_reading_scalars RUNTIME)

add_executable(iotools_atlas iotools_atlas.cpp)
target_link_libraries(iotools_atlas PRIVATE ROOT::ROOTDataFrame)
install(TARGETS iotools_atlas RUNTIME)

add_executable(tree2ntuple tree2ntuple.cpp)
target_link_libraries(tree2ntuple PRIVATE ROOT::ROOTNTupleUtil)
install(TARGETS tree2ntuple RUNTIME)

install(FILES run.fish TYPE BIN PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE)
