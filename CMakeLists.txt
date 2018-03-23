cmake_minimum_required (VERSION 2.8)

set (CMAKE_CXX_STANDARD 14)

include_directories (include)

aux_source_directory (lib/sona SONA_SRC)

add_library (sona ${SONA_SRC})

add_executable (temporary driver/temporarymain.cc)
target_link_libraries (temporary sona)
