#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstring>

using namespace std;

typedef struct {
    int id;
    char palavra[100];
} Registro;

int carregar_dados(const char *nome_arquivo, Registro **vetor) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir a lista de exclusao: %s!\n", nome_arquivo);
        return -1;
    }

    int capacidade = 10;
    int quantidade = 0;
    *vetor = (Registro *) malloc(capacidade * sizeof(Registro));

    if (*vetor == NULL) {
        fclose(arquivo);
        return -1;
    }

    while (fscanf(arquivo, "%d,%99[^\n]\n", &(*vetor)[quantidade].id, (*vetor)[quantidade].palavra) == 2) {
        quantidade++;
        if (quantidade >= capacidade) {
            capacidade *= 2;
            Registro *temp = (Registro *) realloc(*vetor, capacidade * sizeof(Registro));
            if (temp == NULL) {
                fclose(arquivo);
                return quantidade;
            }
            *vetor = temp;
        }
    }

    fclose(arquivo);
    return quantidade;
}

int busca_binaria_palavra(Registro *vetor, int tamanho, const string &palavra_buscada) {
    int inicio = 0;
    int fim = tamanho - 1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        int comp = palavra_buscada.compare(vetor[meio].palavra);

        if (comp == 0) return meio;
        if (comp > 0) inicio = meio + 1;
        else fim = meio - 1;
    }

    return -1;
}

int main() {
    Registro *stop_words = NULL;
    int total_stop_words = carregar_dados("dados.txt", &stop_words);

    if (total_stop_words <= 0) {
        cout << "Aviso: Nenhuma stop-word carregada ou arquivo dados.txt ausente." << endl;
    } else {
        cout << "Sucesso: " << total_stop_words << " stop-words carregadas." << endl;
    }

    ifstream arquivo("livro.txt");

    if (!arquivo.is_open()) {
        cout << "Erro! O arquivo livro.txt nao abriu!" << endl;
        free(stop_words);
        return 1;
    }

    string linha;
    int numeroDaLinha = 1;

    while (getline(arquivo, linha)) {
        stringstream leitorDeLinha(linha);
        string palavraSuja;

        while (leitorDeLinha >> palavraSuja) {
            string palavraLimpa = "";
            for (char c : palavraSuja) {
                if (isalnum((unsigned char)c)) {
                    palavraLimpa += tolower((unsigned char)c);
                }
            }

            if (!palavraLimpa.empty()) {
                int eh_proibida = busca_binaria_palavra(stop_words, total_stop_words, palavraLimpa);

                if (eh_proibida == -1) {
                    cout << "[Linha " << numeroDaLinha << "] Palavra aceita: " << palavraLimpa << endl;
                }
            }
        }
        numeroDaLinha++;
    }

    arquivo.close();
    free(stop_words);
    return 0;
}