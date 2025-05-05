#include <iostream>
#include <vector>
#include "BranchAndBound.h"
#include "ProblemReader.h"

int main() {
    try {
        ProblemReader reader;
        glp_prob* raw_problem = reader.read("/app/tests/teste3_19.txt");

        // Wrap the raw pointer in a unique_ptr
        using GLPProbPtr = std::unique_ptr<glp_prob, GLPKProbDeleter>;
        GLPProbPtr problem(raw_problem, GLPKProbDeleter());

        BranchAndBound bb;
        std::vector<int> solution;
        double objective_value = 0.0;

        SolveStatus status = bb.solve(std::move(problem), solution, objective_value, true); // true for DFS
        if (status == SolveStatus::OK) {
            std::cout << "Optimal solution found: z = " << objective_value << "\n";
            for (size_t i = 0; i < solution.size(); ++i) {
                std::cout << "x" << (i + 1) << " = " << solution[i] << "\n";
            }
        } else {
            std::cerr << "Failed to solve the problem.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}


