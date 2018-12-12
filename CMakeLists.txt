cmake_minimum_required (VERSION 2.8)

project(ckxc-v2)

set (CMAKE_CXX_STANDARD 14)

# this is temporary
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
endif ()

# this is also temporary
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif ()

include_directories (include)

aux_source_directory (lib/Basic BASIC_SRC)
aux_source_directory (lib/sona SONA_SRC)
aux_source_directory (lib/AST AST_SRC)
aux_source_directory (lib/Syntax SYNTAX_SRC)
aux_source_directory (lib/Sema SEMA_SRC)

add_library (sona ${SONA_SRC})
add_library (AST ${AST_SRC})
add_library (Syntax ${SYNTAX_SRC})
add_library (Sema ${SEMA_SRC})
add_library (Basic ${BASIC_SRC})

add_executable (temporary driver/temporarymain.cc)
target_link_libraries (temporary sona)
target_link_libraries (temporary AST)
target_link_libraries (temporary Syntax)
target_link_libraries (temporary Sema)

target_link_libraries (Syntax sona)

