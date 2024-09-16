# at the behest of CMake warnings
project(GraphCompleterTest)
cmake_minimum_required(VERSION 3.20)

# NOTE: to build, run:
# cd <this_directory>
# mkdir build
# cmake -B build
# cmake --build build
# then, run /test/runtests.bat (or a similar batch script that you write yourself for unix platforms and, inmportantly, run from the same directory) to run the tests (and hopefully see no failed ones)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(graphtestlib
	./src/graphcompleter.hpp
	./src/graphcompleter.cpp)

target_include_directories(graphtestlib PUBLIC include)

add_executable(csvtester0 ./test/cpp/csvtest0.cpp)
add_executable(csvtester1 ./test/cpp/csvtest1.cpp)
add_executable(csvtester2 ./test/cpp/csvtest2.cpp)
add_executable(csvtester3 ./test/cpp/csvtest3.cpp)
add_executable(csvtester4 ./test/cpp/csvtest4.cpp)
add_executable(csvtester5 ./test/cpp/csvtest5.cpp)
add_executable(csvtester6 ./test/cpp/csvtest6.cpp)

target_link_libraries(csvtester0 PRIVATE graphtestlib)
target_link_libraries(csvtester1 PRIVATE graphtestlib)
target_link_libraries(csvtester2 PRIVATE graphtestlib)
target_link_libraries(csvtester3 PRIVATE graphtestlib)
target_link_libraries(csvtester4 PRIVATE graphtestlib)
target_link_libraries(csvtester5 PRIVATE graphtestlib)
target_link_libraries(csvtester6 PRIVATE graphtestlib)