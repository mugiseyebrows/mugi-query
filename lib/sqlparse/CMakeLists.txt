cmake_minimum_required(VERSION 3.14)

project(sqlparse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)
qt_standard_project_setup()

add_library(sqlparse SHARED
  sqlparse_global.h
  sqlparse.cpp
  sqlparse.h
)

target_link_libraries(sqlparse PRIVATE Qt::Core)

target_compile_definitions(sqlparse PRIVATE SQLPARSE_LIBRARY)

qt_add_executable(tst_sqlparse tst_sqlparse.cpp)
add_test(NAME tst_sqlparse COMMAND tst_sqlparse)
target_link_libraries(tst_sqlparse PRIVATE Qt::Test sqlparse)
target_include_directories(tst_sqlparse PRIVATE ".")
