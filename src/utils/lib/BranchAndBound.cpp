#include "BranchAndBound.h"
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <iostream> // Para logs

namespace {
    constexpr double INTEGER_TOLERANCE = 1e-6;
}

// Construtor principal
Node::Node(
    std::unique_ptr<GLPKSolver> solver,
    std::vector<std::pair<int, int>> fixed,
    double b,
    NodeType t
) : solver(std::move(solver)),
    fixed_vars(std::move(fixed)),
    bound(b),
    type(t) {}

// Construtor de movimento
Node::Node(Node&& other) noexcept
    : solver(std::move(other.solver)),
      fixed_vars(std::move(other.fixed_vars)),
      bound(other.bound),
      type(other.type) {}

// Operador de atribuição por movimento
Node& Node::operator=(Node&& other) noexcept {
    if (this != &other) {
        solver = std::move(other.solver);
        fixed_vars = std::move(other.fixed_vars);
        bound = other.bound;
        type = other.type;
    }
    return *this;
}

// Construtor da classe BranchAndBound
BranchAndBound::BranchAndBound() 
    : best_objective_(-std::numeric_limits<double>::infinity()) {}

// Copia um problema GLPK
GLPProbPtr BranchAndBound::copyProblem(glp_prob* original) const {
    if (!original) {
        std::cerr << "[BranchAndBound][ERRO] Não é possível copiar um problema nulo." << std::endl;
        throw std::invalid_argument("Não é possível copiar um problema nulo");
    }
    std::cout << "[BranchAndBound][INFO] Copiando problema GLPK." << std::endl;
    GLPProbPtr copy(glp_create_prob());
    if (!copy) {
        std::cerr << "[BranchAndBound][ERRO] Falha ao criar um novo problema GLPK." << std::endl;
        throw std::runtime_error("Falha ao criar um novo problema GLPK");
    }
    glp_copy_prob(copy.get(), original, GLP_ON);
    std::cout << "[BranchAndBound][INFO] Problema copiado com sucesso." << std::endl;
    return copy;
}

/// @brief Função auxiliar que adiciona restrições fixas ao problema GLPK.
/// @param lp 
/// @param fixed_vars 
void BranchAndBound::addFixedConstraints(
    glp_prob* lp, 
    const std::vector<std::pair<int, int>>& fixed_vars) const {
    if (!lp) {
        std::cerr << "[BranchAndBound][AVISO] Problema GLPK nulo fornecido para adicionar restrições." << std::endl;
        return;
    }
    std::cout << "[BranchAndBound][INFO] Adicionando restrições fixas ao problema." << std::endl;
    for (const auto& [var_idx, fixed_value] : fixed_vars) {
        std::cout << "[BranchAndBound][DEBUG] Fixando variável x" << var_idx + 1 
                  << " ao valor " << fixed_value << "." << std::endl;
        glp_set_col_bnds(lp, var_idx + 1, GLP_FX, fixed_value, fixed_value);
    }
}

// Verifica se a solução é inteira
SolveStatus BranchAndBound::solutionIsInteger(
    const std::vector<double>& solution, 
    int& fractional_var) const {
    std::cout << "[BranchAndBound][INFO] Verificando se a solução é inteira." << std::endl;
    fractional_var = selectBranchingVariable(solution);
    if (fractional_var == -1) {
        std::cout << "[BranchAndBound][INFO] Solução é inteira." << std::endl;
        for (double val : solution) {
            if (val < -INTEGER_TOLERANCE || val > 1.0 + INTEGER_TOLERANCE) {
                std::cerr << "[BranchAndBound][AVISO] Solução é inviável." << std::endl;
                return SolveStatus::INFEASIBLE;
            }
        }
        return SolveStatus::OK;
    }
    std::cout << "[BranchAndBound][INFO] Solução é fracionária." << std::endl;
    return SolveStatus::FRACTIONAL;
}

// Seleciona a variável de ramificação
int BranchAndBound::selectBranchingVariable(const std::vector<double>& solution) const {
    int fractional_var = -1;
    double min_dist = 0.5;

    // Verifica se todas as variáveis estão em [0,1]
    for (size_t i = 0; i < solution.size(); ++i) {
        double value = solution[i];
        if (value < -INTEGER_TOLERANCE || value > 1.0 + INTEGER_TOLERANCE) {
            std::cout << "[BranchAndBound][DEBUG] Variável x" << i + 1 
                      << " inviável: " << value << std::endl;
            return -1; // Nó inviável
        }
    }

    // Busca variável fracionária mais próxima de 0.5
    for (size_t i = 0; i < solution.size(); ++i) {
        double value = solution[i];
        double dist = std::abs(value - 0.5);
        if (dist < min_dist && (value > INTEGER_TOLERANCE && value < 1.0 - INTEGER_TOLERANCE)) {
            min_dist = dist;
            fractional_var = static_cast<int>(i);
        }
    }

    return fractional_var;
}

// Cria nós filhos a partir de um nó atual
void BranchAndBound::createChildNodes(Node&& current_node,
                                    int branching_var,
                                    std::function<void(Node&&)> node_processor) {
    if (branching_var == -1) {
        std::cerr << "[BranchAndBound][AVISO] Variável de ramificação inválida (-1)." << std::endl;
        return;
    }
    std::cout << "[BranchAndBound][INFO] Criando nós filhos para a variável de ramificação x" 
              << branching_var + 1 << "." << std::endl;

    // Cria nó filho esquerdo (x_j = 0)
    auto left_problem = copyProblem(current_node.solver->getProblem());
    std::vector<std::pair<int, int>> left_fixed = current_node.fixed_vars;
    left_fixed.emplace_back(branching_var, 0);
    auto left_solver = std::make_unique<GLPKSolver>(std::move(left_problem));
    node_processor(Node(std::move(left_solver), std::move(left_fixed), 
                      current_node.bound, NodeType::LEFT_CHILD));
    std::cout << "[BranchAndBound][DEBUG] Nó filho esquerdo criado com x" << branching_var + 1 << " = 0." << std::endl;

    // Cria nó filho direito (x_j = 1)
    auto right_problem = copyProblem(current_node.solver->getProblem());
    std::vector<std::pair<int, int>> right_fixed = current_node.fixed_vars;
    right_fixed.emplace_back(branching_var, 1);
    auto right_solver = std::make_unique<GLPKSolver>(std::move(right_problem));
    node_processor(Node(std::move(right_solver), std::move(right_fixed), 
                  current_node.bound, NodeType::RIGHT_CHILD));
    std::cout << "[BranchAndBound][DEBUG] Nó filho direito criado com x" << branching_var + 1 << " = 1." << std::endl;
}

// Verifica se a solução é viável
bool BranchAndBound::isSolutionFeasible(glp_prob* problem, const std::vector<int>& solution) const {
    const int num_rows = glp_get_num_rows(problem);
    for (int row = 1; row <= num_rows; ++row) {
        double lhs = 0.0;
        const int num_cols = glp_get_num_cols(problem);
        int* indices = new int[num_cols + 1];
        double* coefs = new double[num_cols + 1];
        int len = glp_get_mat_row(problem, row, indices, coefs);
        
        for (int i = 1; i <= len; ++i) {
            int col = indices[i];
            lhs += coefs[i] * solution[col - 1];
        }
        
        delete[] indices;
        delete[] coefs;
        
        const double rhs = glp_get_row_ub(problem, row);
        if (lhs > rhs + INTEGER_TOLERANCE) {
            return false;
        }
    }
    return true;
}

// Resolve o problema usando o algoritmo Branch and Bound
SolveStatus BranchAndBound::solve(
    GLPProbPtr original_problem, 
    std::vector<int>& solution, 
    double& objective_value, 
    bool use_depth_first) {
    if (!original_problem) {
        std::cerr << "[BranchAndBound][ERRO] Problema GLPK nulo fornecido para resolução." << std::endl;
        return SolveStatus::ERROR;
    }

    std::cout << "[BranchAndBound][INFO] Iniciando o algoritmo Branch and Bound." << std::endl;
    best_objective_ = -std::numeric_limits<double>::infinity();
    best_solution_.clear();

    // Estruturas para BFS (fila) e DFS (pilha)
    std::queue<Node> node_queue;
    std::stack<Node> node_stack;

    // Cria o solver inicial
    auto initial_solver = std::make_unique<GLPKSolver>(std::move(original_problem));
    Node root_node(std::move(initial_solver), 
                 {}, // nenhuma variável fixa
                 std::numeric_limits<double>::infinity(),  
                 NodeType::ROOT);

    // Adiciona o nó raiz à fila ou pilha
    if (use_depth_first) {
        std::cout << "[BranchAndBound][INFO] Usando busca em profundidade." << std::endl;
        node_stack.push(std::move(root_node));
    } else {
        std::cout << "[BranchAndBound][INFO] Usando busca em largura." << std::endl;
        node_queue.push(std::move(root_node));
    }

    // Função que será usada para processar nós filhos
    auto process_node = [&](Node&& node) {
        if (use_depth_first) {
            node_stack.push(std::move(node));
        } else {
            node_queue.push(std::move(node));
        }
    };

    while (!(use_depth_first ? node_stack.empty() : node_queue.empty())) {
        std::cout << "[BranchAndBound][INFO] Processando o próximo nó." << std::endl;
        Node current_node = use_depth_first ? 
            std::move(node_stack.top()) : 
            std::move(node_queue.front());
    
        if (use_depth_first) {
            node_stack.pop();
        } else {
            node_queue.pop();
        }

        // Adiciona restrições de variáveis fixas
        addFixedConstraints(current_node.solver->getProblem(), current_node.fixed_vars);

        // Resolve a relaxação linear
        std::cout << "[BranchAndBound][INFO] Resolvendo a relaxação linear." << std::endl;
        const int num_cols = glp_get_num_cols(current_node.solver->getProblem());
        std::vector<double> relaxed_solution(num_cols);
        double current_objective;
        
        // Resolve o problema
        SolveStatus solve_status = current_node.solver->solve(relaxed_solution, current_objective);

        // Verifica se o nó é viável e se o limite é promissor
        if (solve_status != SolveStatus::OK || current_objective <= best_objective_) {
            std::cout << "[BranchAndBound][DEBUG] Nó podado devido a limite não promissor." << std::endl;
            continue; // Poda o nó
        }

        // Verifica se a solução é inteira
        int fractional_var;
        SolveStatus integer_status = solutionIsInteger(relaxed_solution, fractional_var);

        if (integer_status == SolveStatus::OK) {
            std::cout << "[BranchAndBound][INFO] Solução inteira encontrada com valor objetivo " 
                      << current_objective << "." << std::endl;
            std::vector<int> candidate_solution(relaxed_solution.size());
            std::transform(
                relaxed_solution.begin(), relaxed_solution.end(),
                candidate_solution.begin(),
                [](double val) { return static_cast<int>(std::round(val)); }
            ); // Apenas arredondando a solução de double para int
            
            if (isSolutionFeasible(current_node.solver->getProblem(), candidate_solution)) {
                if (current_objective > best_objective_) {
                    std::cout << "[BranchAndBound][INFO] Atualizando a melhor solução encontrada." << std::endl;
                    best_objective_ = current_objective;
                    best_solution_ = candidate_solution;
                }
            }
            continue;
        } else if (integer_status == SolveStatus::FRACTIONAL) {
            std::cout << "[BranchAndBound][INFO] Solução fracionária encontrada. Criando nós filhos." << std::endl;
            createChildNodes(std::move(current_node), fractional_var, process_node);
        }
    }

    if (best_solution_.empty()) {
        std::cerr << "[BranchAndBound][ERRO] Nenhuma solução viável encontrada." << std::endl;
        return SolveStatus::ERROR;
    }

    std::cout << "[BranchAndBound][INFO] Melhor solução encontrada com valor objetivo " 
              << best_objective_ << "." << std::endl;
    solution = best_solution_;
    objective_value = best_objective_;
    return SolveStatus::OK;
}