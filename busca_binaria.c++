#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para representar o registro de dados (exemplo de acervo/itens)
typedef struct {
    int id;
    char nome[100];
} Registro;

// Função para ler o arquivo e preencher o vetor dinâmico
int carregar_dados(const char *nome_arquivo, Registro **vetor) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s!\n", nome_arquivo);
        return -1;
    }

    int capacidade = 10;
    int quantidade = 0;
    *vetor = (Registro *) malloc(capacidade * sizeof(Registro));

    if (*vetor == NULL) {
        printf("Erro de alocacao de memoria!\n");
        fclose(arquivo);
        return -1;
    }

    // Leitura formatada do arquivo (id,nome)
    while (fscanf(arquivo, "%d,%99[^\n]\n", &(*vetor)[quantidade].id, (*vetor)[quantidade].nome) == 2) {
        quantidade++;
        
        // Realocação dinâmica de memória caso o vetor encha
        if (quantidade >= capacidade) {
            capacidade *= 2;
            Registro *temp = (Registro *) realloc(*vetor, capacidade * sizeof(Registro));
            if (temp == NULL) {
                printf("Erro ao realocar memoria!\n");
                fclose(arquivo);
                return quantidade;
            }
            *vetor = temp;
        }
    }

    fclose(arquivo);
    return quantidade; // Retorna a quantidade total de registros lidos
}

// Algoritmo de Busca Binária
// Pré-requisito: O vetor precisa estar ordenado pelo campo 'id'
int busca_binaria(Registro *vetor, int tamanho, int id_buscado) {
    int inicio = 0;
    int fim = tamanho - 1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;

        if (vetor[meio].id == id_buscado) {
            return meio; // Elemento encontrado (retorna o índice)
        }

        if (vetor[meio].id < id_buscado) {
            inicio = meio + 1; // Busca na metade direita
        } else {
            fim = meio - 1;    // Busca na metade esquerda
        }
    }

    return -1; // Elemento não encontrado
}

int main() {
    Registro *dados = NULL;
    const char *arquivo_caminho = "dados.txt";

    int total_registros = carregar_dados(arquivo_caminho, &dados);

    if (total_registros <= 0) {
        printf("Nenhum dado carregado ou erro na leitura.\n");
        return 1;
    }

    printf("Sucesso: %d registros carregados do arquivo.\n\n", total_registros);

    // Teste da Busca Binaria
    int chave_busca = 105; // Altere para o ID que deseja buscar
    int resultado = busca_binaria(dados, total_registros, chave_busca);

    if (resultado != -1) {
        printf("[SUCESSO] ID %d encontrado no indice %d!\n", chave_busca, resultado);
        printf("Nome do Registro: %s\n", dados[resultado].nome);
    } else {
        printf("[FALHA] ID %d nao foi encontrado nos dados.\n", chave_busca);
    }

    // Liberacao de memoria alocada
    free(dados);
    return 0;
}