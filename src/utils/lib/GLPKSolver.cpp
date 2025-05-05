#include "GLPKSolver.h"
#include <stdexcept>
#include <string>
#include <iostream> // Para logs


/// @brief Functor que será utilizado para deletar o problema GLPK.
/// @details Este functor é utilizado como um deleter para o std::unique_ptr, garantindo que o problema GLPK
/// seja deletado corretamente quando o ponteiro único for destruído.
void GLPKProbDeleter::operator()(glp_prob* ptr) const noexcept {
    if (ptr) {
        std::cout << "[DEBUG] Deletando o problema GLPK." << std::endl;
        glp_delete_prob(ptr);
    }
}

/// @brief Construtor padrão.
/// @details Este construtor cria uma instância do GLPKSolver a partir de um ponteiro para um problema GLPK.
/// @param problem 
GLPKSolver::GLPKSolver(GLPProbPtr problem) 
    : problem_(std::move(problem)) { // Carrega o problema GLPK antes de qualquer outra operação
    if (!problem_) {
        std::cerr << "[ERROR] Falha ao carregar a instância do problema GLPK: ponteiro nulo fornecido." << std::endl;
        throw std::runtime_error("Failed to load GLPK problem instance: null pointer provided");
    }
    std::cout << "[INFO] Instância do problema GLPK carregada com sucesso." << std::endl;
}

/// @brief Resolve o problema GLPK.
    /// @details Este método resolve o problema GLPK e armazena a solução e o valor objetivo.
    /// @param solution Vetor onde a solução será armazenada.
    /// @param objective_value Variável onde o valor objetivo será armazenado.
    /// @return O status da solução: OK, INFEASIBLE, UNBOUNDED, ERROR, FRACTIONAL
SolveStatus GLPKSolver::solve(std::vector<double>& solution, double& objective_value) {
    if (!problem_) {
        std::cerr << "[ERROR] A instância do problema GLPK é nula." << std::endl;
        throw std::runtime_error("GLPK problem instance is null");
    }

    // Verificando se o vetor de solução tem tamanho adequado
    const int num_cols = glp_get_num_cols(problem_.get());
    std::cout << "[INFO] Número de colunas no problema: " << num_cols << std::endl;
    if (solution.size() < static_cast<size_t>(num_cols)) {
        std::cout << "[DEBUG] Redimensionando o vetor de solução para " << num_cols << " elementos." << std::endl;
        solution.resize(num_cols);
    }

    // Resolver o problema
    std::cout << "[INFO] Iniciando a resolução do problema com glp_simplex." << std::endl;
    int ret = glp_simplex(problem_.get(), nullptr);
    if (ret != 0) {
        std::cerr << "[ERROR] Falha ao resolver o problema com glp_simplex. Código de retorno: " << ret << std::endl;
        return SolveStatus::ERROR;
    }

    // Verificar status da solução
    const int status = glp_get_status(problem_.get());
    std::cout << "[INFO] Status da solução obtido: " << status << std::endl;
    switch (status) {
        case GLP_OPT:
            std::cout << "[INFO] Solução ótima encontrada." << std::endl;
            break;
        case GLP_INFEAS:
            std::cerr << "[WARNING] O problema é inviável." << std::endl;
            return SolveStatus::INFEASIBLE;
        case GLP_NOFEAS:
            std::cerr << "[WARNING] Nenhuma solução viável encontrada." << std::endl;
            return SolveStatus::INFEASIBLE;
        case GLP_UNBND:
            std::cerr << "[WARNING] O problema é ilimitado." << std::endl;
            return SolveStatus::UNBOUNDED;
        default:
            std::cerr << "[ERROR] Status desconhecido da solução: " << status << std::endl;
            return SolveStatus::ERROR;
    }

    // Obter a solução
    try {
        std::cout << "[INFO] Extraindo a solução primária e o valor objetivo." << std::endl;
        for (int i = 1; i <= num_cols; ++i) {
            solution[i - 1] = glp_get_col_prim(problem_.get(), i);
            std::cout << "[DEBUG] Variável x" << i << " = " << solution[i - 1] << std::endl;
        }
        objective_value = glp_get_obj_val(problem_.get());
        std::cout << "[INFO] Valor objetivo: " << objective_value << std::endl;
    } catch (...) {
        std::cerr << "[ERROR] Erro ao extrair a solução ou o valor objetivo." << std::endl;
        return SolveStatus::ERROR;
    }
    
    std::cout << "[INFO] Resolução concluída com sucesso." << std::endl;
    return SolveStatus::OK;
}