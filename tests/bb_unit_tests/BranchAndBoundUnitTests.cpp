#include <gtest/gtest.h>
#include <vector>
#include "BranchAndBound.h"
#include "ProblemReader.h"

class BranchAndBoundTest : public ::testing::TestWithParam<std::tuple<std::string, double, bool>> {
protected:
    // Helper function to run the test case
    void RunTest(const std::string& filename, double expected_objective, bool use_depth_first) {
        try {
            ProblemReader reader;
            glp_prob* raw_problem = reader.read(filename);

            // Wrap the raw pointer in a unique_ptr
            using GLPProbPtr = std::unique_ptr<glp_prob, GLPKProbDeleter>;
            GLPProbPtr problem(raw_problem, GLPKProbDeleter());

            BranchAndBound bb;
            std::vector<int> solution;
            double objective_value = 0.0;

            SolveStatus status = bb.solve(std::move(problem), solution, objective_value, use_depth_first);
            ASSERT_EQ(int(status), int(SolveStatus::OK)) << "Failed to solve the problem: " << filename;

            EXPECT_NEAR(objective_value, expected_objective, 1e-6)
                << "Incorrect objective value for: " << filename;
        } catch (const std::exception& e) {
            FAIL() << "Exception thrown for " << filename << ": " << e.what();
        }
    }
};

// Test cases
INSTANTIATE_TEST_SUITE_P(
    BranchAndBoundTests,
    BranchAndBoundTest,
    ::testing::Values(
        std::make_tuple("/app/tests/teste1_20.txt", 20.0, true),
        std::make_tuple("/app/tests/teste1_20.txt", 20.0, false),
        std::make_tuple("/app/tests/teste2_24.txt", 24.0, true),
        std::make_tuple("/app/tests/teste2_24.txt", 24.0, false),
        std::make_tuple("/app/tests/teste3_19.txt", 19.0, true),
        std::make_tuple("/app/tests/teste3_19.txt", 19.0, false),
        std::make_tuple("/app/tests/teste4_10.txt", 10.0, true),
        std::make_tuple("/app/tests/teste4_10.txt", 10.0, false)
    )
);

TEST_P(BranchAndBoundTest, ParametrizedTest) {
    auto [filename, expected_objective, use_depth_first] = GetParam();
    RunTest(filename, expected_objective, use_depth_first);
}
