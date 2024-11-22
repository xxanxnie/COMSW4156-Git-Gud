cmake_minimum_required(VERSION 3.15)

project(GitGud)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")

# Set paths for external libraries
set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/external_libraries/boost)
set(CROW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external_libraries/Crow/include)
set(ASIO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external_libraries/asio/include)

# MongoDB C++ Driver paths
if (APPLE)
    set(MONGOCXX_LIB_PATH "/usr/local/lib/libmongocxx.dylib")
    set(BSONCXX_LIB_PATH "/usr/local/lib/libbsoncxx.dylib")
else()
    set(MONGOCXX_LIB_PATH "/usr/local/lib/libmongocxx.so")
    set(BSONCXX_LIB_PATH "/usr/local/lib/libbsoncxx.so")
endif()

set(BCRYPT_INCLUDE_DIR "/usr/local/include")
set(BCRYPT_LIBRARY "/usr/local/lib/libbcrypt.a")

# Add spdlog
find_package(spdlog CONFIG REQUIRED)
message(STATUS "spdlog version: ${spdlog_VERSION}")

# Include directories
set(INCLUDE_PATHS 
    ${BOOST_ROOT}
    ${CROW_INCLUDE_DIR}
    ${ASIO_INCLUDE_DIR}
    /usr/local/include/mongocxx/v_noabi
    /usr/local/include/bsoncxx/v_noabi
    /usr/local/include/libmongoc-1.0
    /usr/local/include/libbson-1.0
    /usr/local/include/bcrypt
    /usr/local/include/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc
    include
)

# Add these lines to find MongoDB C++ driver
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${MONGO_CXX_ROOT}/build)

set(SOURCE_FILES 
    src/main.cpp 
    src/RouteController.cpp
    src/DatabaseManager.cpp
    src/services/Counseling.cpp
    src/services/Food.cpp
    src/services/Healthcare.cpp
    src/services/Outreach.cpp
    src/services/Shelter.cpp
    src/Auth.cpp
)

set(TEST_FILES 
    test/CounselingUnitTests.cpp
    test/FoodUnitTests.cpp
    test/HealthcareUnitTests.cpp
    test/OutreachUnitTests.cpp
    test/ShelterUnitTests.cpp
    test/RouteControllerUnitTests.cpp
    #test/AuthUnitTests.cpp
)

set(SOURCE_FILES_NO_MAIN
    src/RouteController.cpp
    src/DatabaseManager.cpp
    src/services/Counseling.cpp
    src/services/Food.cpp
    src/services/Healthcare.cpp
    src/services/Outreach.cpp
    src/services/Shelter.cpp
    src/Auth.cpp
)

# Add bcrypt to your include paths
list(APPEND INCLUDE_PATHS ${BCRYPT_INCLUDE_DIR})

# Main project executable
add_executable(GitGud ${SOURCE_FILES})

# find_package(BCrypt REQUIRED)
# target_link_libraries(GitGud PRIVATE BCrypt)

find_package(jwt-cpp CONFIG REQUIRED)
target_link_libraries(GitGud PRIVATE jwt-cpp::jwt-cpp)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(GitGud PRIVATE -Wno-deprecated-declarations)
endif()

# Google Test setup
include(FetchContent)
FetchContent_Declare(
    googletest
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    URL https://github.com/google/googletest/archive/ff233bdd4cac0a0bf6e5cd45bda3406814cb2796.zip
)
FetchContent_MakeAvailable(googletest)

# Include directories for main executable
target_include_directories(GitGud PUBLIC ${INCLUDE_PATHS})

# Link libraries
target_link_libraries(GitGud PRIVATE 
    ${BCRYPT_LIBRARY}
    gtest 
    gtest_main
    gmock
    gmock_main
    ${MONGOCXX_LIB_PATH}
    ${BSONCXX_LIB_PATH}
    jwt-cpp::jwt-cpp
    spdlog::spdlog
)

enable_testing()

# Test executable
add_executable(GitGudTests ${TEST_FILES} ${SOURCE_FILES_NO_MAIN})

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(GitGudTests PRIVATE -Wno-deprecated-declarations)
endif()

target_include_directories(GitGudTests PRIVATE ${INCLUDE_PATHS})

target_link_libraries(GitGudTests PRIVATE 
    ${BCRYPT_LIBRARY}
    gtest 
    gtest_main
    gmock
    gmock_main
    ${MONGOCXX_LIB_PATH}
    ${BSONCXX_LIB_PATH}
    jwt-cpp::jwt-cpp
    spdlog::spdlog
)

include(GoogleTest)
add_test(NAME GitGudTests COMMAND GitGudTests)
set_tests_properties(GitGudTests PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=/usr/local/lib:$ENV{LD_LIBRARY_PATH}")

# Add custom target for coverage analysis

add_custom_target(
    coverage
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
