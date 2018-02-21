cmake_minimum_required(VERSION 3.0)
project (CppLangUtils)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

add_definitions(-DBOOST_TEST_DYN_LINK)

enable_testing()

file(GLOB test_srcs src/unit_test/*.cpp)

foreach (test_src ${test_srcs})
  get_filename_component(test_name ${test_src} NAME_WE)
  add_executable(${test_name} ${test_src})
  add_test(
    NAME ${test_name}
    COMMAND ${CMAKE_BINARY_DIR}/${test_name})
endforeach()


  
