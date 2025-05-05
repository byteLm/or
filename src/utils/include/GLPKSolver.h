#ifndef GLPK_SOLVER_H
#define GLPK_SOLVER_H

#include <vector>
#include <memory>
#include <glpk.h>
#include <stdexcept>
#include <string>
#include "SolveStatus.h"


/// @brief Functor que será utilizado para deletar o problema GLPK.
/// @details Este functor é utilizado como um deleter para o std::unique_ptr, garantindo que o problema GLPK
/// seja deletado corretamente quando o ponteiro único for destruído.
struct GLPKProbDeleter {
    void operator()(glp_prob* ptr) const noexcept;
};

using GLPProbPtr = std::unique_ptr<glp_prob, GLPKProbDeleter>;

class GLPKSolver {
public:
    /// @brief Construtor padrão.
    /// @details Este construtor cria uma instância do GLPKSolver a partir de um ponteiro para um problema GLPK.
    /// @param problem 
    explicit GLPKSolver(GLPProbPtr problem);
    
    // Isso garante que o solver não seja copiado
    GLPKSolver(const GLPKSolver&) = delete;
    GLPKSolver& operator=(const GLPKSolver&) = delete;
    
    // Permite movimento
    // Ou seja, ao utilizar std::move, o ponteiro do problema GLPK será transferido
    // para o novo objeto, e o objeto original não terá mais acesso a ele (evitando cópias desnecessárias e dangling pointer)
    GLPKSolver(GLPKSolver&&) noexcept = default;
    GLPKSolver& operator=(GLPKSolver&&) noexcept = default;
    
    /// @brief Destrutor padrão.
    /// @details Este destrutor é responsável por liberar os recursos alocados para o problema GLPK.
    /// @details O problema GLPK será deletado automaticamente quando o ponteiro único for destruído.
    ~GLPKSolver() = default;
    

    /// @brief Resolve o problema GLPK.
    /// @details Este método resolve o problema GLPK e armazena a solução e o valor objetivo.
    /// @param solution Vetor onde a solução será armazenada.
    /// @param objective_value Variável onde o valor objetivo será armazenado.
    /// @return O status da solução: OK, INFEASIBLE, UNBOUNDED, ERROR, FRACTIONAL
    SolveStatus solve(std::vector<double>& solution, double& objective_value);
    
    /// @brief Retorna o ponteiro para o problema GLPK.
    /// @details Este método retorna o ponteiro para o problema GLPK.
    /// @return Um ponteiro para o problema GLPK.
    glp_prob* getProblem() noexcept { return problem_.get(); }
    const glp_prob* getProblem() const noexcept { return problem_.get(); }
    
    /// @brief Libera o problema GLPK.
    /// @details Este método libera o problema GLPK e retorna o ponteiro único.
    /// @return Um ponteiro único para o problema GLPK.
    /// @details O ponteiro único será transferido para o chamador, e o objeto atual não terá mais acesso a ele.
    GLPProbPtr releaseProblem() noexcept { return std::move(problem_); }
    
private:
    /// @brief Ponteiro único para o problema GLPK.
    /// @details Este ponteiro único é responsável por gerenciar a memória do problema GLPK. Segue: <glp_prob, GLPKProbDeleter>
    GLPProbPtr problem_;
};

#endif // GLPK_SOLVER_H