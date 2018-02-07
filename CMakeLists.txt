cmake_minimum_required(VERSION 3.0)
project (CppLangUtils)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

add_definitions(-DBOOST_TEST_DYN_LINK)
add_executable(test_tuple ${PROJECT_SOURCE_DIR}/src/unit_test/test_tuple.cpp)
enable_testing()
add_test(
  NAME test_tuple
  COMMAND ${CMAKE_BINARY_DIR}/test_tuple)

  
