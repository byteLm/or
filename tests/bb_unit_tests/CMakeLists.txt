

# test project
project(BranchAndBoundTests)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(BranchAndBoundTests
    BranchAndBoundUnitTests.cpp
    ${SRC_FILES}
)

# Link with GTest and your other dependencies
target_link_libraries(BranchAndBoundTests 
    PRIVATE 
    GTest::GTest 
    GTest::Main 
    glpk::glpk)

gtest_discover_tests(BranchAndBoundTests)
