#ifndef PROBLEM_READER_H
#define PROBLEM_READER_H

#include <string>
#include <stdexcept>
#include <glpk.h>
#include <fstream>
#include <sstream>

/// @brief Classe responsável por ler o arquivo problema e criar uma instância do GLPK.
class ProblemReader {
public:
    /// @brief Construtor padrão (vazio).
    ProblemReader() {}

    /// @brief Função principal que lê o arquivo e cria o problema GLPK.
    /// @param filepath O caminho do arquivo a ser lido.
    /// @return Um ponteiro para o problema GLPK criado.
    glp_prob* read(const std::string &filepath);
};

#endif // PROBLEM_READER_H
