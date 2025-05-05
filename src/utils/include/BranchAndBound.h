#ifndef BRANCH_AND_BOUND_H
#define BRANCH_AND_BOUND_H

#include <vector>
#include <utility>
#include <memory>
#include <functional>
#include "GLPKSolver.h"

// Enum para representar o tipo de nó
enum class NodeType {
    ROOT,
    LEFT_CHILD,  // x_j = 0
    RIGHT_CHILD  // x_j = 1
};

// Estrutura para representar um nó na árvore de Branch and Bound
struct Node {
    std::unique_ptr<GLPKSolver> solver;
    std::vector<std::pair<int, int>> fixed_vars;
    double bound;
    NodeType type;

    // Construtor principal
    Node(
        std::unique_ptr<GLPKSolver> solver,
        std::vector<std::pair<int, int>> fixed,
        double b,
        NodeType t
    );

    // Construtor de movimento
    Node(Node&& other) noexcept;

    // Operações de cópia desabilitadas
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    // Operador de atribuição por movimento
    Node& operator=(Node&& other) noexcept;
};

// Classe principal para o algoritmo Branch and Bound
class BranchAndBound {
public:
    explicit BranchAndBound();

    // Operações de cópia desabilitadas
    BranchAndBound(const BranchAndBound&) = delete;
    BranchAndBound& operator=(const BranchAndBound&) = delete;

    // Permitir operações de movimento
    BranchAndBound(BranchAndBound&&) noexcept = default;
    BranchAndBound& operator=(BranchAndBound&&) noexcept = default;

    ~BranchAndBound() = default;

    // Método principal para resolver o problema
    SolveStatus solve(
        GLPProbPtr original_problem, 
        std::vector<int>& solution, 
        double& objective_value, 
        bool use_depth_first = true
    );

private:
    double best_objective_;
    std::vector<int> best_solution_;

    // Métodos auxiliares
    GLPProbPtr copyProblem(glp_prob* original) const;
    SolveStatus solutionIsInteger(
        const std::vector<double>& solution, 
        int& fractional_var
    ) const;
    void addFixedConstraints(
        glp_prob* lp, 
        const std::vector<std::pair<int, int>>& fixed_vars
    ) const;
    int selectBranchingVariable(const std::vector<double>& solution) const;

    // Função auxiliar para criar nós filhos
    void createChildNodes(
        Node&& current_node,
        int branching_var,
        std::function<void(Node&&)> node_processor
    );

    // Verifica se a solução é viável
    bool isSolutionFeasible(
        glp_prob* problem, 
        const std::vector<int>& solution
    ) const;
};

#endif // BRANCH_AND_BOUND_H