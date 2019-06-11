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
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wextra -fno-delayed-template-parsing")
    if (CMAKE_BUILD_TYPE MATCHES "Release")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    endif()
endif ()

include_directories (include)
include_directories (test)

aux_source_directory (lib/Basic BASIC_SRC)
aux_source_directory (lib/sona SONA_SRC)
aux_source_directory (lib/AST AST_SRC)
aux_source_directory (lib/Syntax SYNTAX_SRC)
aux_source_directory (lib/Sema SEMA_SRC)
aux_source_directory (lib/Frontend FRONTEND_SRC)
aux_source_directory (lib/Backend BACKEND_SRC)

add_library (sona ${SONA_SRC})
add_library (AST ${AST_SRC})
add_library (Syntax ${SYNTAX_SRC})
add_library (Sema ${SEMA_SRC})
add_library (Basic ${BASIC_SRC})
add_library (Frontend ${FRONTEND_SRC})
add_library (Backend ${BACKEND_SRC})

add_executable (temporary driver/temporarymain.cc)
target_link_libraries (temporary Sema AST Frontend Syntax Backend Basic sona)

add_executable (ckx-syntax driver/SynCheck.cc)
target_link_libraries (ckx-syntax Sema AST Frontend Syntax Basic sona)

add_executable (ckx-ast driver/ASTDump.cc)
target_link_libraries (ckx-ast Frontend Backend Sema AST Syntax Basic sona)

add_executable (TestLex test/Frontend/LexTest.cc)
target_link_libraries (TestLex Frontend Syntax Basic sona)

add_executable (TestParse test/Frontend/ParseTest.cc)
target_link_libraries (TestParse Frontend Syntax Basic sona)

add_executable (TestSemaBasis test/Sema/SemaBasisTest.cc)
target_link_libraries (TestSemaBasis Sema Frontend Syntax AST Basic sona)

add_executable (TestDepResolve test/Sema/DepResolveTest.cc)
target_link_libraries (TestDepResolve Sema Frontend Syntax AST Basic sona)

add_executable (TestPostSubstDep test/Sema/PostSubstDepTest.cc)
target_link_libraries (TestPostSubstDep Sema Frontend Syntax AST Basic sona)

add_executable(TestLookup test/Sema/LookupTest.cc)
target_link_libraries (TestLookup Sema Frontend Syntax AST Basic sona)
