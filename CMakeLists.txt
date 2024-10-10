cmake_minimum_required(VERSION 3.15)

project(GitGud)

set(MONGO_CXX_DRIVER_PATH "/opt/homebrew/include/mongocxx/v_noabi")
set(MONGO_C_DRIVER_PATH "/opt/homebrew/include/libmongoc-1.0")

set(INCLUDE_PATHS 
    external_libraries/boost 
    external_libraries/Crow/include
    external_libraries/asio/include
    ${MONGO_CXX_DRIVER_PATH}
    ${MONGO_C_DRIVER_PATH}
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")

set(SOURCE_FILES 
    src/main.cpp 
    src/RouteController.cpp
    src/DatabaseManager.cpp

    src/services/Counseling.cpp
    src/services/Food.cpp
    src/services/Healthcare.cpp
    src/services/Outreach.cpp
    src/services/Shelter.cpp
)

set(TEST_FILES 
    test/CounselingUnitTests.cpp
    test/FoodUnitTests.cpp
    test/HealthcareUnitTests.cpp
    test/OutreachUnitTests.cpp
    test/ShelterUnitTests.cpp
    test/RouteControllerUnitTests.cpp
)

# Main project executable
add_executable(GitGud ${SOURCE_FILES})

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/ff233bdd4cac0a0bf6e5cd45bda3406814cb2796.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

# Include directories for main executable
target_include_directories(GitGud PUBLIC 
    ${INCLUDE_PATHS} 
    include
    #/opt/homebrew/Cellar/asio/1.30.211
    /opt/homebrew/opt/asio/include
)

target_link_libraries(GitGud PRIVATE 
    gtest 
    gtest_main
)

enable_testing()

# Test executable
add_executable(GitGudTests ${TEST_FILES} ${SOURCE_FILES})

target_include_directories(GitGudTests PRIVATE 
    ${INCLUDE_PATHS} 
    include
    #/opt/homebrew/opt/asio/1.30.2/include
    /opt/homebrew/opt/asio/include
)

target_link_libraries(GitGudTests PRIVATE 
    gtest 
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(GitGudTests)

add_custom_target(
    run_tests
    COMMAND $<TARGET_FILE:GitGudTests>
    DEPENDS GitGudTests
    COMMENT "Running tests to generate coverage data"
)

# Add custom target for coverage analysis
add_custom_target(
    coverage
    DEPENDS run_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles/GitGudTests.dir/src
    COMMAND gcov *.cpp.gcno
    COMMAND lcov --capture --directory . --output-file Coverage.info --ignore-errors inconsistent --filter range
    COMMAND lcov --remove Coverage.info "/usr*" "include/" -o FilteredCoverage.info
    COMMAND genhtml FilteredCoverage.info --output-directory UnitTestCoverageOutput --ignore-errors inconsistent --filter range 
    COMMAND open UnitTestCoverageOutput/index.html
    COMMENT "Generating code coverage report"
)

# Find the cpplint program
find_program(CPPLINT cpplint)

if (CPPLINT)
    # Custom target to run cpplint
    add_custom_target(
        cpplint
        COMMAND ${CPPLINT} --filter=-whitespace,-build/include_what_you_use ${SOURCE_FILES} ${TEST_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running cpplint for Google C++ Style Guide compliance"
    )
else()
    message(WARNING "cpplint not found! Skipping style checks.")
endif()