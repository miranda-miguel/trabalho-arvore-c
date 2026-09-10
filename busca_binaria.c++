#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <chrono>

using namespace std;

// Grau da Árvore B+ (Permite testar 3, 4, 5 e 6)
const int ORDEM = 3; 
const int MAX_CHAVES = ORDEM - 1;

// ==========================================
// ESTRUTURA DE DADOS E STOP-WORDS (FILTRO)
// ==========================================

struct Palavra {
    string palavra;
    int ocorrencias;
    vector<int> linhas;
};

// Variavel global para armazenar as stop-words ordenadas
vector<string> stopWords;

// Transforma palavra em minuscula e remove pontuacao
string limparPalavra(string palavra) {
    string limpa = "";
    for (char c : palavra) {
        if (isalnum(static_cast<unsigned char>(c))) {
            limpa += tolower(static_cast<unsigned char>(c));
        }
    }
    return limpa;
}

// Carrega o arquivo de stop-words e ordena para a Busca Binaria
void carregarStopWords(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Aviso: Nao foi possivel abrir o arquivo de stop-words: " << nomeArquivo << endl;
        return;
    }
    string palavra;
    while (arquivo >> palavra) {
        palavra = limparPalavra(palavra);
        if (!palavra.empty()) {
            stopWords.push_back(palavra);
        }
    }
    arquivo.close();
    sort(stopWords.begin(), stopWords.end());
}

// Busca Binaria na lista de Stop-Words
bool ehStopWord(const string& palavra) {
    int inicio = 0;
    int fim = stopWords.size() - 1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        if (stopWords[meio] == palavra) return true;
        if (stopWords[meio] < palavra) inicio = meio + 1;
        else fim = meio - 1;
    }
    return false;
}

// ==========================================
// IMPLEMENTACAO DA ARVORE B+ (CODIGO DO GRUPO)
// ==========================================

struct NoBMais {
    bool folha;
    int numChaves;
    string chaves[MAX_CHAVES];
    Palavra dados[MAX_CHAVES];
    NoBMais* filhos[ORDEM + 1];
    NoBMais* proximo;

    NoBMais(bool ehFolha) {
        folha = ehFolha;
        numChaves = 0;
        proximo = nullptr;
        for (int i = 0; i < ORDEM + 1; i++) {
            filhos[i] = nullptr;
        }
    }
};

Palavra* buscar(NoBMais* raiz, const string& palavra) {
    if (raiz == nullptr) return nullptr;

    if (raiz->folha) {
        for (int i = 0; i < raiz->numChaves; i++) {
            if (raiz->chaves[i] == palavra) {
                return &raiz->dados[i];
            }
        }
        return nullptr;
    }

    int i = 0;
    while (i < raiz->numChaves && palavra >= raiz->chaves[i]) {
        i++;
    }
    return buscar(raiz->filhos[i], palavra);
}

void adicionarOcorrencia(Palavra& palavra, int linha) {
    palavra.ocorrencias++;
    if (palavra.linhas.empty() || palavra.linhas.back() != linha) {
        palavra.linhas.push_back(linha);
    }
}

void inserirNaFolha(NoBMais* folha, const Palavra& palavra) {
    int i = folha->numChaves - 1;
    while (i >= 0 && palavra.palavra < folha->chaves[i]) {
        folha->chaves[i + 1] = folha->chaves[i];
        folha->dados[i + 1] = folha->dados[i];
        i--;
    }
    folha->chaves[i + 1] = palavra.palavra;
    folha->dados[i + 1] = palavra;
    folha->numChaves++;
}

bool inserirRecursivo(NoBMais* no, const Palavra& palavra, string& chavePromovida, NoBMais*& novoNo) {
    if (no->folha) {
        for (int i = 0; i < no->numChaves; i++) {
            if (no->chaves[i] == palavra.palavra) {
                adicionarOcorrencia(no->dados[i], palavra.linhas[0]);
                return false;
            }
        }

        if (no->numChaves < MAX_CHAVES) {
            inserirNaFolha(no, palavra);
            return false;
        }

        vector<Palavra> temporario;
        for (int i = 0; i < no->numChaves; i++) temporario.push_back(no->dados[i]);
        temporario.push_back(palavra);

        sort(temporario.begin(), temporario.end(), [](const Palavra& a, const Palavra& b) {
            return a.palavra < b.palavra;
        });

        NoBMais* novaFolha = new NoBMais(true);
        int meio = temporario.size() / 2;

        no->numChaves = 0;
        for (int i = 0; i < meio; i++) {
            no->dados[i] = temporario[i];
            no->chaves[i] = temporario[i].palavra;
            no->numChaves++;
        }

        for (int i = meio; i < (int)temporario.size(); i++) {
            int pos = i - meio;
            novaFolha->dados[pos] = temporario[i];
            novaFolha->chaves[pos] = temporario[i].palavra;
            novaFolha->numChaves++;
        }

        novaFolha->proximo = no->proximo;
        no->proximo = novaFolha;
        chavePromovida = novaFolha->chaves[0];
        novoNo = novaFolha;
        return true;
    }

    int i = 0;
    while (i < no->numChaves && palavra.palavra >= no->chaves[i]) {
        i++;
    }

    string chaveNova;
    NoBMais* filhoNovo = nullptr;
    bool dividiu = inserirRecursivo(no->filhos[i], palavra, chaveNova, filhoNovo);

    if (!dividiu) return false;

    if (no->numChaves < MAX_CHAVES) {
        for (int j = no->numChaves; j > i; j--) {
            no->chaves[j] = no->chaves[j - 1];
            no->filhos[j + 1] = no->filhos[j];
        }
        no->chaves[i] = chaveNova;
        no->filhos[i + 1] = filhoNovo;
        no->numChaves++;
        return false;
    }

    vector<string> chavesTemporarias;
    vector<NoBMais*> filhosTemporarios;

    for (int j = 0; j < no->numChaves; j++) chavesTemporarias.push_back(no->chaves[j]);
    for (int j = 0; j <= no->numChaves; j++) filhosTemporarios.push_back(no->filhos[j]);

    chavesTemporarias.insert(chavesTemporarias.begin() + i, chaveNova);
    filhosTemporarios.insert(filhosTemporarios.begin() + i + 1, filhoNovo);

    NoBMais* novoInterno = new NoBMais(false);
    int meio = chavesTemporarias.size() / 2;
    chavePromovida = chavesTemporarias[meio];

    no->numChaves = 0;
    for (int j = 0; j < meio; j++) {
        no->chaves[j] = chavesTemporarias[j];
        no->filhos[j] = filhosTemporarios[j];
        no->numChaves++;
    }
    no->filhos[meio] = filhosTemporarios[meio];

    int pos = 0;
    for (int j = meio + 1; j < (int)chavesTemporarias.size(); j++) {
        novoInterno->chaves[pos] = chavesTemporarias[j];
        novoInterno->filhos[pos] = filhosTemporarios[j];
        novoInterno->numChaves++;
        pos++;
    }
    novoInterno->filhos[pos] = filhosTemporarios.back();
    novoNo = novoInterno;

    return true;
}

void inserir(NoBMais*& raiz, const string& palavra, int linha) {
    Palavra* existente = buscar(raiz, palavra);
    if (existente != nullptr) {
        adicionarOcorrencia(*existente, linha);
        return;
    }

    Palavra novaPalavra;
    novaPalavra.palavra = palavra;
    novaPalavra.ocorrencias = 1;
    novaPalavra.linhas.push_back(linha);

    if (raiz == nullptr) {
        raiz = new NoBMais(true);
        inserirNaFolha(raiz, novaPalavra);
        return;
    }

    string chavePromovida;
    NoBMais* novoNo = nullptr;
    bool dividiu = inserirRecursivo(raiz, novaPalavra, chavePromovida, novoNo);

    if (dividiu) {
        NoBMais* novaRaiz = new NoBMais(false);
        novaRaiz->chaves[0] = chavePromovida;
        novaRaiz->filhos[0] = raiz;
        novaRaiz->filhos[1] = novoNo;
        novaRaiz->numChaves = 1;
        raiz = novaRaiz;
    }
}

// Leitura com Filtro por Busca Binaria
void lerArquivo(NoBMais*& raiz, const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Erro ao abrir o arquivo do livro: " << nomeArquivo << "\n";
        return;
    }

    string linhaTexto;
    int numeroLinha = 1;

    while (getline(arquivo, linhaTexto)) {
        string palavra = "";
        for (char c : linhaTexto) {
            if (isalnum(static_cast<unsigned char>(c))) {
                palavra += tolower(static_cast<unsigned char>(c));
            } else {
                if (!palavra.empty()) {
                    // FILTRO: So insere se NAO for stop-word
                    if (!ehStopWord(palavra)) {
                        inserir(raiz, palavra, numeroLinha);
                    }
                    palavra.clear();
                }
            }
        }
        if (!palavra.empty()) {
            if (!ehStopWord(palavra)) {
                inserir(raiz, palavra, numeroLinha);
            }
        }
        numeroLinha++;
    }
    arquivo.close();
}

void consultar(NoBMais* raiz, string palavra) {
    palavra = limparPalavra(palavra);

    auto inicio = chrono::high_resolution_clock::now();
    Palavra* resultado = buscar(raiz, palavra);
    auto fim = chrono::high_resolution_clock::now();

    chrono::duration<double, micro> tempo = fim - inicio;

    if (resultado == nullptr) {
        cout << "\nA palavra \"" << palavra << "\" nao foi encontrada (ou e uma stop-word).\n";
        cout << "Tempo de busca: " << tempo.count() << " microssegundos\n";
        return;
    }

    cout << "\n-----------------------------\n";
    cout << "Palavra: " << resultado->palavra << endl;
    cout << "Ocorrencias: " << resultado->ocorrencias << endl;
    cout << "Linhas: ";
    for (size_t i = 0; i < resultado->linhas.size(); i++) {
        cout << resultado->linhas[i];
        if (i + 1 < resultado->linhas.size()) cout << ", ";
    }
    cout << "\nTempo de busca: " << tempo.count() << " microssegundos\n";
    cout << "-----------------------------\n";
}

void mostrarTodas(NoBMais* raiz) {
    if (raiz == nullptr) {
        cout << "Arvore vazia.\n";
        return;
    }

    NoBMais* atual = raiz;
    while (!atual->folha) atual = atual->filhos[0];

    cout << "\n===== PALAVRAS INDEXADAS =====\n";
    while (atual != nullptr) {
        for (int i = 0; i < atual->numChaves; i++) {
            cout << atual->dados[i].palavra << " - " << atual->dados[i].ocorrencias << " ocorrencia(s)\n";
        }
        atual = atual->proximo;
    }
}

void liberar(NoBMais* no) {
    if (no == nullptr) return;
    if (!no->folha) {
        for (int i = 0; i <= no->numChaves; i++) liberar(no->filhos[i]);
    }
    delete no;
}

// ==========================================
// MAIN COM CARREGAMENTO DO FILTRO
// ==========================================

int main() {
    NoBMais* raiz = nullptr;

    // 1. Carrega as Stop-words via busca binaria
    carregarStopWords("dados.txt");

    string nomeArquivo;
    cout << "Digite o nome do arquivo .txt (ex: livro.txt): ";
    cin >> nomeArquivo;

    // 2. Le o arquivo e popula a Arvore B+ filtrada
    lerArquivo(raiz, nomeArquivo);

    if (raiz == nullptr) {
        cout << "Nenhuma palavra valida foi inserida.\n";
        return 0;
    }

    int opcao;
    do {
        cout << "\n==============================\n";
        cout << "   INDEXADOR - ARVORE B+ (Grau " << ORDEM << ")\n";
        cout << "==============================\n";
        cout << "1 - Consultar palavra\n";
        cout << "2 - Mostrar todas as palavras\n";
        cout << "0 - Sair\n";
        cout << "Escolha: ";
        cin >> opcao;

        switch (opcao) {
            case 1: {
                string palavra;
                cout << "Digite a palavra: ";
                cin >> palavra;
                consultar(raiz, palavra);
                break;
            }
            case 2:
                mostrarTodas(raiz);
                break;
            case 0:
                cout << "Programa encerrado.\n";
                break;
            default:
                cout << "Opcao invalida.\n";
        }
    } while (opcao != 0);

    liberar(raiz);
    return 0;
}