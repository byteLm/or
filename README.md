# Implementação - Algoritmo Branch and Bound em C++ utilizando GLPK
## Vídeo de apresentação: https://www.loom.com/share/fb13d25a263b4faeb514fc8ffe304802?sid=c6342916-81fc-42ed-aa1a-4f8c030dd85a

[![C++](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B)
[![Docker](https://img.shields.io/badge/Docker-20.10.7-blue.svg)](https://www.docker.com/)
[![Conan](https://img.shields.io/badge/Conan-1.35.0-blue.svg)](https://conan.io/)

## Sobre

Este repositório é uma implementação do algoritmo Branch and Bound em C++ utilizando a biblioteca GLPK Este é um trabalho acadêmico desenvolvido como parte do curso de Introdução à Pesquisa Operacional, ministrada pelo professor Teobaldo Bulhões (UFPB).


## Ferramentas Utilizadas

O algoritmo foi implementado em C++ e utiliza a biblioteca GLPK (apenas para resolução de subproblemas de PLI). O ambiente de desenvolvimento foi preparado da seguinte forma:

- **Dev Container C++ / Docker**: O projeto foi desenvolvido em um contêiner do Visual Studio Code, utilizando a extensão Remote - Containers. O contêiner é baseado na imagem `mcr.microsoft.com/devcontainers/cpp:latest`, que já possui o compilador g++, cmake e outras ferramentas necessárias para o desenvolvimento em C++ - certas modificações foram feitas, como a instalação do python3/conan, essas modificações estão em `.devcontainer/Dockerfile`.

- **Conan**: O Conan é um gerenciador de pacotes para C++ que facilita a instalação e gerenciamento de dependências. O projeto utiliza o Conan para instalar a biblioteca COIN-OR CLP. O arquivo `conanfile.txt`, "similar a requirements.txt" do Python, lista as dependências do projeto. 

- **VSCode Tasks**: O arquivo `.vscode-template/tasks.json` pré-define comando úteis para compilar e executar o projeto, como o `conan install`.

## Primeiros passos
1. **Instalação do Docker**: Certifique-se de ter o Docker instalado em sua máquina. Você pode baixar o Docker Desktop [aqui](https://www.docker.com/products/docker-desktop).

2. **Instale a extensão Remote - Containers**: No Visual Studio Code, instale a extensão Remote - Containers. Isso permitirá que você abra o projeto dentro do contêiner Docker.
3. **Clone o repositório**: Clone este repositório em sua máquina local.

```bash
git clone ...
```	
4. **Abra o projeto no VSCode**: Abra o Visual Studio Code e, em seguida, abra a pasta do projeto clonado.
5. (Opcional) **Renomeie a pasta `vscode-template` para `.vscode`**: Se você quiser usar as tasks pré-definidas.
6. **Abra o projeto no contêiner**: Clique no ícone do canto inferior esquerdo do VSCode (o ícone de "><") e selecione "Reabrir pasta no contêiner". Isso iniciará o contêiner Docker e abrirá o projeto dentro dele.
7. **Instale as dependências**: Após abrir o projeto no contêiner, realiza a task `Conan Fetch Dependencies` para que o Conan instale as dependências listadas no arquivo `conanfile.txt`. Você pode fazer isso pressionando `Ctrl + Shift + P` e digitando "Run Task", em seguida, selecione a task `Conan Fetch Dependencies`.
8. **Build do projeto**: O Conan irá gerar um arquivo `CMakePresets.json`, este arquivo é copiado automaticamente pela task para o workspace. Dessa forma, você pode compilar o projeto utilizando a extensão CMake (já contida no devcontainer), sem demais configurações. Para isso, pressione `Ctrl + Shift + P` e digite "CMake: Build". Isso irá compilar o projeto e gerar os executáveis na pasta `build`.

## Referências
[Pesquisa Operacional - Teobaldo Bulhões](https://sites.google.com/view/po-ufpb/in%C3%ADcio)
