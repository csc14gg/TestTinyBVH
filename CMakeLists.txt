cmake_minimum_required(VERSION 3.20)

project(TestTinyBvhProject VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard (optional, but recommended)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX") # Enable warnings as errors

# Set output directories (optional, but good practice)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "include/*.h" "include/*.hpp")

# Create executable
add_executable(TestTinyBvh ${SOURCES})

# Install target (optional)
install(TARGETS TestTinyBvh
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# Install headers (optional)
install(DIRECTORY include/ DESTINATION include)

# Optional: Add compiler definitions.
# target_compile_definitions(TestTinyBvh PRIVATE MY_DEFINITION)

# Optional: Add include directories.
# target_include_directories(TestTinyBvh PRIVATE include)

# Optional: Link against external libraries.
# target_link_libraries(TestTinyBvh PRIVATE my_external_library)

# Optional: Enable CTest integration.
# enable_testing()
# add_test(NAME TestTinyBvhTests COMMAND TestTinyBvh)