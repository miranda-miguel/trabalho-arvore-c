# Indexador de Palavras em C++

Trabalho de Estrutura de Dados 2 - Professor Denis Gonçalves Cople  
UERJ-ZO - Curso de Tecnologia em Análise e Desenvolvimento de Sistemas  

## 📌 Descrição
O projeto realiza a leitura e indexação de um arquivo de texto, permitindo contar palavras, verificar sua ocorrência e listar as linhas em que aparecem. 

Palavras irrelevantes (*stop-words*) são filtradas via busca binária em uma lista ordenada antes de serem armazenadas nas estruturas de dados em árvore:
* **Árvore AVL**
* **Árvore B+** (Graus 3, 4, 5 e 6)

## 🚀 Como Compilar e Executar

### Pré-requisitos
* Compilador C++ (`g++` / MinGW)

### Passo a passo
1. Compile o código no terminal:
   ```bash
   g++ busca_binaria.c++ -o programa.exe