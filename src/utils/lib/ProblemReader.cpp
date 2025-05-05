
#include "ProblemReader.h"

glp_prob* ProblemReader::read(const std::string &filepath) {

    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open the file: " + filepath);
    }

    // Ponteiro com o problema que será retornado
    glp_prob *lp = glp_create_prob();

    // Pelos requisitos do trabalho, o problema é sempre de maximização
    glp_set_obj_dir(lp, GLP_MAX); 


    // Lendo a primeira linha do arquivo
    std::string line;
    int num_vars, num_constraints;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Failed to read the first line of the file.");
    }

    // Lendo o número de variáveis e restrições
    std::istringstream iss(line);
    if (!(iss >> num_vars >> num_constraints)) {
        throw std::invalid_argument("Invalid format in the first line of the file.");
    }
    if (num_vars <= 0 || num_constraints <= 0) {
        throw std::out_of_range("Number of variables or constraints is out of range.");
    }

    glp_add_rows(lp, num_constraints);
    glp_add_cols(lp, num_vars);

    // Coeficientes da função objetivo
    if (!std::getline(file, line)) {
        throw std::runtime_error("Failed to read the objective function line.");
    }
    std::istringstream obj_iss(line);
    for (int i = 1; i <= num_vars; ++i) {
        double coef;
        if (!(obj_iss >> coef)) {
            throw std::invalid_argument("Invalid format in the objective function line.");
        }
        glp_set_obj_coef(lp, i, coef);
    }

    // Leitura das restrições do problema
    for (int i = 1; i <= num_constraints; ++i) {

        // Obtém a linha
        if (!std::getline(file, line)) {
            throw std::runtime_error("Falha ao ler a linha da restrição " + std::to_string(i) + ".");
        }
        std::istringstream constraint_iss(line);

        // Arrays para armazenar os índices e coeficientes das variáveis na restrição
        int indices[num_vars + 1];
        double coefficients[num_vars + 1];

        // Lê os coeficientes das variáveis na restrição
        for (int j = 1; j <= num_vars; ++j) {
            if (!(constraint_iss >> coefficients[j])) {
                throw std::invalid_argument("Formato inválido na linha da restrição " + std::to_string(i) + ".");
            }
            indices[j] = j; // Define o índice da variável
        }

        // Lê o lado direito da restrição (valor limite)
        double rhs;
        if (!(constraint_iss >> rhs)) {
            throw std::invalid_argument("Formato inválido na linha da restrição " + std::to_string(i) + ".");
        }

        // Define os coeficientes e o lado direito da restrição no problema GLPK
        glp_set_mat_row(lp, i, num_vars, indices, coefficients);
        glp_set_row_bnds(lp, i, GLP_UP, 0.0, rhs); // Define a restrição como "menor ou igual a rhs"
    }

    // Define o intervalo [0, 1] para cada variável
    for (int i = 1; i <= num_vars; ++i) {
        glp_set_col_bnds(lp, i, GLP_DB, 0.0, 1.0);
    }

    return lp;
}

