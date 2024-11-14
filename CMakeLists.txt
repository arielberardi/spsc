cmake_minimum_required(VERSION 3.28.3)
project(ScSp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


### DEPENDENCIES ###
find_package(Boost REQUIRED)

include(FetchContent)

# Google Test Suite
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
)

FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest googlebenchmark)

include(GoogleTest)

### BUILD PROJECT ####

include_directories(${PROJECT_SOURCE_DIR}/Source)

file(GLOB SOURCES "Source/*.cpp")

set(LIBRARIES
  pthread
)

# Main Build
add_compile_options(-Wall -Werror -Wextra)

add_executable(${PROJECT_NAME} ${SOURCES} Source/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})

add_executable(${PROJECT_NAME}.tsan ${SOURCES} Source/main.cpp)
target_compile_options(${PROJECT_NAME}.tsan PRIVATE -fsanitize=thread) 
target_link_libraries(${PROJECT_NAME}.tsan PRIVATE ${LIBRARIES} tsan) 

### BUILD TESTS ###
enable_testing()

set(GOOGLE_TEST_LIBRARIES
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  asan
  ubsan
)

set(TEST_FILES
  RingBuffer_Mutex
  RingBuffer_Atomic
)

foreach(file ${TEST_FILES})
  add_executable(${file}_test Tests/${file}_test.cpp)
  target_compile_options(${file}_test PRIVATE -fsanitize=undefined -fsanitize=address)
  target_link_libraries(${file}_test PRIVATE ${GOOGLE_TEST_LIBRARIES} ${LIBRARIES})
  target_link_options(${file}_test PRIVATE -fsanitize=undefined -fsanitize=address)
  gtest_discover_tests(${file}_test)
endforeach()

### BUILD BENCH ###
set(BENCH_FILES
  RingBuffer_Mutex
  RingBuffer_Atomic
  RingBuffer_Boost
)

foreach(file ${BENCH_FILES})
  add_executable(${file}_bench Tests/${file}_bench.cpp)
  target_link_libraries(${file}_bench benchmark::benchmark ${GOOGLE_TEST_LIBRARIES} ${LIBRARIES})
endforeach()
