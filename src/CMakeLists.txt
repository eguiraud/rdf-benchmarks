message(STATUS "Looking for ROOT")
find_package(ROOT REQUIRED COMPONENTS ROOTDataFrame)
message(STATUS "ROOT ${ROOT_VERSION} found at ${ROOT_BINDIR}")

add_executable(without_io without_io.cpp)
target_link_libraries(without_io PRIVATE ROOT::ROOTDataFrame)
install(TARGETS without_io RUNTIME)

add_executable(dimuonanalysis dimuonanalysis.cpp)
target_link_libraries(dimuonanalysis PRIVATE ROOT::ROOTDataFrame)
install(TARGETS dimuonanalysis RUNTIME)

install(FILES run.fish TYPE BIN PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE)
