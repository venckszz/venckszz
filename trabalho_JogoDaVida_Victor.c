/****************************************************************************************
    Trabalho Computacional - Programação Estruturada/Programação II - 2025/1
    <Victor Da Rocha Toniato>
*****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    FILE *arquivo_entrada;
    FILE *arquivo_saida;
    int linhas;
    int colunas;
    int numeros_geracao;
    int **Matriz;
    char nome_arquivo[50];
} Tabuleiro;

FILE *abre_arquivo(Tabuleiro *pTab, char *nome_arquivo);
FILE *cria_arquivo_saida(char *nome_arquivo);
void cria_tabuleiro(Tabuleiro *pTab);
void preenche_tabuleiro(int linhas, int colunas, int **matriz);
void remove_newline(char elemento[]);
void nome_arquivo(int linhas, int colunas, Tabuleiro *pTab);
int converte_estado(char estado);
void roda_geracoes(FILE* arquivo_saida, int **matriz, int linhas, int colunas, int numeros_geracao);
int conta_vizinhos(int aux1, int aux2, int linhas, int colunas, int **matriz);
int regras_jogo(int vizinhos, int linha, int coluna, int **matriz);
void imprime_saida(int aux, int linhas, int colunas, int** matriz, int** estado_futuro, FILE *arquivo_saida);
int** aloca_memoria(int linhas, int colunas);
void desaloca_memoria(int **matriz, int linhas);


int main(void)
{
    int opcao;
    Tabuleiro tab = {0}; // Inicializa todos os valores = 0 para evitar erros
    do {
        printf("Menu de opcoes:\n");
        printf("    1 - Abrir arquivo de entrada\n");
        printf("    2 - Preencher tabuleiro de entrada\n");
        printf("    3 - Rodar geracoes\n");
        printf("    Escolha sua opcao (-1 para sair): ");
        scanf("%d", &opcao);
        getchar(); // Limpa o \n do scanf

        switch (opcao) {
            case 1:

                if (tab.Matriz != NULL) {
                    // Desaloca memoria caso o usuario escolha a opcao +1 vez
                    desaloca_memoria(tab.Matriz, tab.linhas);
                    tab.Matriz = NULL;
                }

                // Recebe o arquivo da estrutura
                char nome_arquivo_temp[50];
                tab.arquivo_entrada = abre_arquivo(&tab, nome_arquivo_temp);
                if (tab.arquivo_entrada == NULL) {
                    break;
                }

                fscanf(tab.arquivo_entrada, "%d %d %d", &tab.linhas, &tab.colunas, &tab.numeros_geracao);
                tab.Matriz = aloca_memoria(tab.linhas, tab.colunas);

                // Percorre a matriz do arquivo
                for (int i = 0; i < tab.linhas; i++) {
                    for (int j = 0; j < tab.colunas; j++) {
                        char elemento;
                        fscanf(tab.arquivo_entrada, " %c", &elemento);

                        // Converte os chars para int 0 ou 1
                        tab.Matriz[i][j] = converte_estado(elemento);
                    }
                }
                fclose(tab.arquivo_entrada);

                // Cria o arquivo de saida
                tab.arquivo_saida = cria_arquivo_saida(tab.nome_arquivo);
                break;

            case 2:
                if (tab.Matriz != NULL) {
                    // Desaloca memoria caso o usuario escolha a opcao +1 vez
                    desaloca_memoria(tab.Matriz, tab.linhas);
                    tab.Matriz = NULL;
                }
                // Chama a funcao de escrever manualmente a matriz
                cria_tabuleiro(&tab);
                tab.arquivo_saida = cria_arquivo_saida(tab.nome_arquivo);
                preenche_tabuleiro(tab.linhas, tab.colunas, tab.Matriz);
                break;

            case 3:
                if (tab.Matriz == NULL) {
                    printf("Nenhum arquivo de entrada carregado!\n");
                    break;
                }

                // Escreve a matriz antes de comecar as geracoes
                fprintf(tab.arquivo_saida, "Geracao 0:\n");
                for (int i = 0; i < tab.linhas; i++) {
                    for (int j = 0; j < tab.colunas; j++) {
                        if (tab.Matriz[i][j] == 0)
                            fprintf(tab.arquivo_saida,". ");
                        else if (tab.Matriz[i][j] == 1)
                            fprintf(tab.arquivo_saida,"0 ");
                    }
                    fprintf(tab.arquivo_saida, "\n");
                }
                // Roda as geracoes
                roda_geracoes(tab.arquivo_saida, tab.Matriz, tab.linhas, tab.colunas, tab.numeros_geracao);
                fclose(tab.arquivo_saida);
        }

    } while (opcao != -1);

    // Limpa memoria apos encerramento
    if (tab.Matriz != NULL)
        desaloca_memoria(tab.Matriz, tab.linhas);

    return 0;
}

FILE *abre_arquivo(Tabuleiro *pTab, char *nome_arquivo)
{
    int aux = 0;
    printf("\t Insira o nome do arquivo: ");
    fgets(nome_arquivo, 50, stdin);

    // Tira o '\n' do FGETS
    while (nome_arquivo[aux] != '\0') {
        if (nome_arquivo[aux] == '\n') {
            nome_arquivo[aux] = '\0';
            break;
        }
        aux++;
    }
    // Copia o nome do arquivo ate o '.' p/ struct
    strcpy(pTab->nome_arquivo, nome_arquivo);

    // Abre o arquivo para leitura
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("\nFalha ao abrir o arquivo! Voltando ao menu inicial!\n");
        return NULL;
    }
    return arquivo;
}

FILE *cria_arquivo_saida(char *nome_arquivo)
{
    int aux = 0;
    char nome_arquivo_saida[50], temp[50];
    strcpy(temp, nome_arquivo);

    while (temp[aux] != '\0') { // Deixa somente o nome antes do .txt
        if (temp[aux] == '.') {
            temp[aux] = '\0';
            break;
        }
        aux++;
    }
    strcat(temp, "_out.txt"); // Adiciona ao final da str o "_out.txt"
    strcpy(nome_arquivo_saida, temp); // Copia para a variavel final
    FILE *arquivo_saida = fopen(nome_arquivo_saida, "w");
    if (arquivo_saida == NULL) {
        printf("\nFalha ao abrir o arquivo!\n");
        return NULL;
    }
    return arquivo_saida;
}

void cria_tabuleiro(Tabuleiro *pTab)
{
    int linhas, colunas, numeros_geracao;
    printf("\t Insira a quantidade de linhas, colunas e geracoes: ");
    scanf("%d %d %d", &linhas, &colunas, &numeros_geracao);
    getchar(); // Limpa o \n do scanf
    nome_arquivo(linhas, colunas, pTab);

    // Passando por ponteiro os valores para struct
    pTab -> linhas = linhas;
    pTab -> colunas = colunas;
    pTab -> numeros_geracao = numeros_geracao;
    pTab -> Matriz = aloca_memoria(linhas, colunas);
}

void preenche_tabuleiro(int linhas, int colunas, int **matriz)
{
    // Com isso, o programa funciona para matrizes de ate 1000 colunas
    char elemento[1000];

    // Monta a matriz de "strings"
    for (int i = 0; i < linhas; i++) {
        fgets(elemento, 1000, stdin);
        remove_newline(elemento);
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = converte_estado(elemento[j]);
        }
    }
}

int converte_estado(char estado)
{
    if (estado == '.')
        return 0;
    if (estado == '0')
        return 1;
    return -1;
}

void remove_newline(char elemento[])
{
    int temp = 0;
    while (elemento[temp] != '\0') {
        if (elemento[temp] == '\n')
            elemento[temp] = '\0';
        temp++;
    }
}

void nome_arquivo(int linhas, int colunas, Tabuleiro *pTab)
{
    char nome[50];
    sprintf(nome,"%dx%d.txt", linhas, colunas);

    // Passa o nome do arquivo para a struct
    strcpy(pTab -> nome_arquivo, nome);
}

void roda_geracoes(FILE *arquivo_saida, int **matriz, int linhas, int colunas, int numeros_geracao)
{
    // Aloca memoria para o tabuleiro seguinte
    int **tabuleiro_futuro = aloca_memoria(linhas, colunas);
    if (!tabuleiro_futuro) {
        return;
    }
    // Rodando o Jogo da Vida
    for (int aux = 1; aux <= numeros_geracao; aux++) {
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                int vizinhos = 0;

                // Percorre individualmente ao redor de cada celula
                vizinhos = conta_vizinhos(i, j, linhas, colunas, matriz);

                // Valora tabuleiro
                tabuleiro_futuro[i][j] = regras_jogo(vizinhos, i, j, matriz);
            }
        }
        imprime_saida(aux, linhas, colunas, matriz, tabuleiro_futuro, arquivo_saida);
    }
    // Desaloca a memoria da matriz auxiliar para futuras geracoes
    desaloca_memoria(tabuleiro_futuro, linhas);
}

int conta_vizinhos(int aux1, int aux2, int linhas, int colunas, int **matriz) {
    int temp = 0;
    for (int aux_linhas = -1; aux_linhas <= 1; aux_linhas++) {
        for (int aux_colunas = -1; aux_colunas <= 1; aux_colunas++) {

            // Ignora a propria celula
            if (aux_linhas == 0 && aux_colunas == 0)
                continue;

            int linha_vizinha = aux1 + aux_linhas;
            int coluna_vizinha = aux2 + aux_colunas;
            if (linha_vizinha >= 0 && linha_vizinha < linhas && coluna_vizinha >= 0 && coluna_vizinha < colunas)
                temp+= matriz[linha_vizinha][coluna_vizinha];
        }
    }
    return temp;
}

int regras_jogo(int vizinhos, int linha, int coluna, int **matriz)
{
    if (matriz[linha][coluna] == 0) {
        if (vizinhos == 3)
            return 1;
        return 0;
    }
    if (matriz[linha][coluna] == 1) {
        if (vizinhos < 2 || vizinhos > 3)
            return 0;
        return 1;
    }
    return -1; // Nunca chega aqui
}

void imprime_saida(int aux, int linhas, int colunas, int** matriz, int** estado_futuro, FILE *arquivo_saida)
{
    fprintf(arquivo_saida, "Geracao %d:\n", aux);
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = estado_futuro[i][j];

            // Converte para a saida desejada e imprime no arquivo
            if (matriz[i][j] == 1)
                fprintf(arquivo_saida, "0 ");
            else if (matriz[i][j] == 0)
                fprintf(arquivo_saida, ". ");
        }
        fprintf(arquivo_saida, "\n");
    }
}

int** aloca_memoria(int linhas, int colunas)
{
    int **pMatriz = calloc(linhas, sizeof(int*));
    if (pMatriz == NULL) {
        printf("Memoria insuficiente.\n");
        return NULL;
    }

    // Aloca as colunas
    for (int i = 0; i < linhas; i++) {
        pMatriz[i] = calloc(colunas, sizeof(int));

        // Caso a alocacao falhe no meio
        if (pMatriz[i] == NULL) {
            printf("Memoria insuficiente.\n");
            for (int j = 0; j < i; j++) {
                free(pMatriz[j]);
            }
            free(pMatriz);
            return NULL;
        }
    }
    return pMatriz;
}

void desaloca_memoria(int **matriz, int linhas)
{
    // libera o ponteiro das colunas
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    // Libera o ponteiro das linhas
    free(matriz);
}