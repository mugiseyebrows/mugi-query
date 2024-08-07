cmake_minimum_required(VERSION 3.14)

project(emmet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)
qt_standard_project_setup()

add_library(emmet SHARED
  emmet_global.h
  emmet.cpp
  emmet.h
)

target_link_libraries(emmet PRIVATE Qt::Core)

target_compile_definitions(emmet PRIVATE EMMET_LIBRARY)

qt_add_executable(tst_emmet tst_emmet.cpp)
add_test(NAME tst_emmet COMMAND tst_emmet)
target_link_libraries(tst_emmet PRIVATE Qt::Test emmet)
target_include_directories(tst_emmet PRIVATE ".")
