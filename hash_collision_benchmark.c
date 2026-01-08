#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


typedef struct aluno {
    int matricula;
    char nome[30];
    float n1, n2, n3;
} Aluno;


typedef struct hash {
    int TABLE_SIZE;
    Aluno **itens;
    int qtd;
} Hash;


int is_prime(int n) {

    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0) return 0;

    int r = (int) sqrt((double) n);

    for (int i = 3; i <= r; i += 2)
        if (n % i == 0) return 0;
    return 1;
}


int next_prime(int n) {
    while (!is_prime(n)) n++;
    return n;
}


void shuffle(int *v, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}


Hash *criaHash(int TABLE_SIZE) {

    Hash *ha = (Hash *) malloc(sizeof(Hash));

    if (ha == NULL) return NULL;

    ha->TABLE_SIZE = TABLE_SIZE;
    ha->itens = (Aluno **) malloc(TABLE_SIZE * sizeof(Aluno *));

    if (ha->itens == NULL) {
        free(ha);
        return NULL;
    }

    ha->qtd = 0;

    for (int i = 0; i < TABLE_SIZE; i++)
        ha->itens[i] = NULL;

    return ha;
}


void liberaHash(Hash *ha) {
    if (ha == NULL) return;

    for (int i = 0; i < ha->TABLE_SIZE; i++) {
        if (ha->itens[i] != NULL)
            free(ha->itens[i]);
    }

    free(ha->itens);
    free(ha);
}


int chaveMultiplicacao(int chave, int TABLE_SIZE) {
    double A = 0.6180339887;
    double val = chave * A;
    val = val - (int) val;
    return (int) (TABLE_SIZE * val);
}


int sondagemLinear(int pos, int i, int TABLE_SIZE) {
    return ((pos + i) & 0x7FFFFFFF) % TABLE_SIZE;
}


int sondagemQuadratica(int pos, int i, int TABLE_SIZE) {
    pos = pos + 2 * i + 5 * i * i; // c1=2, c2=5
    return (pos & 0x7FFFFFFF) % TABLE_SIZE;
}


int duploHashPos(int H1, int chave, int i, int TABLE_SIZE) {
    int H2 = chaveMultiplicacao(chave, TABLE_SIZE - 1) + 1;
    return ((H1 + i * H2) & 0x7FFFFFFF) % TABLE_SIZE;
}


int insere_linear(Hash *ha, Aluno al, long *count_sondagem, long *count_colisao, int *max_sondagem) {

    if (ha == NULL || ha->qtd == ha->TABLE_SIZE) return 0;

    int chave = al.matricula;
    int pos = chaveMultiplicacao(chave, ha->TABLE_SIZE);
    int primeiro_ocupado = 0;

    for (int i = 0; i < ha->TABLE_SIZE; i++) {

        int newPos = sondagemLinear(pos, i, ha->TABLE_SIZE);
        (*count_sondagem)++;

        if (ha->itens[newPos] == NULL) {

            Aluno *novo = (Aluno *) malloc(sizeof(Aluno));
            if (novo == NULL) return 0;

            *novo = al;
            ha->itens[newPos] = novo;
            ha->qtd++;

            if (i > *max_sondagem) *max_sondagem = i;
            if (primeiro_ocupado) (*count_colisao)++;

            return 1;
        } 
        else {
            if (i == 0) primeiro_ocupado = 1;
        }
    }

    return 0;
}


int insere_quadratica(Hash *ha, Aluno al, long *count_sondagem, long *count_colisao, int *max_sondagem) {
    
    if (ha == NULL || ha->qtd == ha->TABLE_SIZE) return 0;

    int chave = al.matricula;
    int pos = chaveMultiplicacao(chave, ha->TABLE_SIZE);
    int primeiro_ocupado = 0;

    for (int i = 0; i < ha->TABLE_SIZE; i++) {

        int newPos = sondagemQuadratica(pos, i, ha->TABLE_SIZE);
        (*count_sondagem)++;

        if (ha->itens[newPos] == NULL) {
            
            Aluno *novo = (Aluno *) malloc(sizeof(Aluno));
            if (novo == NULL) return 0;

            *novo = al;
            ha->itens[newPos] = novo;
            ha->qtd++;

            if (i > *max_sondagem) *max_sondagem = i;
            if (primeiro_ocupado) (*count_colisao)++;
            return 1;
        } 
        else {
            if (i == 0) primeiro_ocupado = 1;
        }
    }

    return 0;
}


int insere_duplo(Hash *ha, Aluno al, long *count_sondagem, long *count_colisao, int *max_sondagem) {

    if (ha == NULL || ha->qtd == ha->TABLE_SIZE) return 0;

    int chave = al.matricula;
    int H1 = chaveMultiplicacao(chave, ha->TABLE_SIZE);
    int primeiro_ocupado = 0;

    for (int i = 0; i < ha->TABLE_SIZE; i++) {

        int newPos = duploHashPos(H1, chave, i, ha->TABLE_SIZE);
        (*count_sondagem)++;

        if (ha->itens[newPos] == NULL) {

            Aluno *novo = (Aluno *) malloc(sizeof(Aluno));
            if (novo == NULL) return 0;

            *novo = al;
            ha->itens[newPos] = novo;
            ha->qtd++;

            if (i > *max_sondagem) *max_sondagem = i;
            if (primeiro_ocupado) (*count_colisao)++;
            return 1;
        } 
        else {
            if (i == 0) primeiro_ocupado = 1;
        }
    }

    return 0;
}


void reset_table(Hash **ha, int TABLE_SIZE) {
    if (*ha != NULL) liberaHash(*ha);
    *ha = criaHash(TABLE_SIZE);
}


void run_for_N(int N, int reps) {

    printf("=== N = %d (reps = %d) ===\n", N, reps);

    int approx = (int) ceil(N / 0.7);
    int TABLE_SIZE = next_prime(approx);

    int RANGE = N * 10;
    int *pool = (int *) malloc(RANGE * sizeof(int));
    if (pool == NULL) return;
    for (int i = 0; i < RANGE; i++) pool[i] = i + 1;
    
    long total_sondagem_linear = 0, total_sondagem_quadratica = 0, total_sondagem_duplo = 0;
    long total_colisoes_linear = 0, total_colisoes_quadratica = 0, total_colisoes_duplo = 0;
    int total_max_linear = 0, total_max_quad = 0, total_max_duplo = 0;

    for (int t = 0; t < reps; t++) {

        shuffle(pool, RANGE);

        Aluno *arr = (Aluno *) malloc(N * sizeof(Aluno));

        for (int i = 0; i < N; i++) {
            arr[i].matricula = pool[i];
            strcpy(arr[i].nome, "X");
            arr[i].n1 = arr[i].n2 = arr[i].n3 = 0.0f;
        }

        Hash *ht_lin = criaHash(TABLE_SIZE);
        Hash *ht_quad = criaHash(TABLE_SIZE);
        Hash *ht_duplo = criaHash(TABLE_SIZE);

        long sondagem_linear = 0, sondagem_quadratica = 0, sondagem_duplo = 0;
        long colisoes_linear = 0, colisoes_quadratica = 0, colisoes_duplo = 0;
        int max_sondagem_linear = 0, max_sondagem_quadratica = 0, max_sondagem_duplo = 0;

        for (int i = 0; i < N; i++) {
            insere_linear(ht_lin, arr[i], &sondagem_linear, &colisoes_linear, &max_sondagem_linear);

            insere_quadratica(ht_quad, arr[i], &sondagem_quadratica, &colisoes_quadratica, &max_sondagem_quadratica);

            insere_duplo(ht_duplo, arr[i], &sondagem_duplo, &colisoes_duplo, &max_sondagem_duplo);
        }

        total_sondagem_linear += sondagem_linear;
        total_sondagem_quadratica += sondagem_quadratica;
        total_sondagem_duplo += sondagem_duplo;

        total_colisoes_linear += colisoes_linear;
        total_colisoes_quadratica += colisoes_quadratica;
        total_colisoes_duplo += colisoes_duplo;

        total_max_linear += max_sondagem_linear;
        total_max_quad += max_sondagem_quadratica;
        total_max_duplo += max_sondagem_duplo;

        liberaHash(ht_lin);
        liberaHash(ht_quad);
        liberaHash(ht_duplo);
        free(arr);
    }

    double avg_sondagem_linear = (double) total_sondagem_linear / reps;
    double avg_sondagem_quadratica = (double) total_sondagem_quadratica / reps;
    double avg_sondagem_duplo = (double) total_sondagem_duplo / reps;

    double avg_colisoes_linear = (double) total_colisoes_linear / reps;
    double avg_colisoes_quadratica = (double) total_colisoes_quadratica / reps;
    double avg_colisoes_duplo = (double) total_colisoes_duplo / reps;

    double avg_max_lin = (double) total_max_linear / reps;
    double avg_max_quad = (double) total_max_quad / reps;
    double avg_max_duplo = (double) total_max_duplo / reps;

    printf("TABLE_SIZE escolhido = %d (fator de carga aproximado = %.2f)\n", TABLE_SIZE, (double) N / (double) TABLE_SIZE);
    printf("Metodo      | avg_total_sondagem | avg_sondagem/insercao | avg_colisao | avg_max_colisao\n");
    printf("---------------------------------------------------------------------------------\n");
    printf("Linear      | %16.0f | %15.4f | %14.2f | %13.2f\n",
           avg_sondagem_linear, avg_sondagem_linear / N, avg_colisoes_linear, avg_max_lin);
    printf("Quadratico   | %16.0f | %15.4f | %14.2f | %13.2f\n",
           avg_sondagem_quadratica, avg_sondagem_quadratica / N, avg_colisoes_quadratica, avg_max_quad);
    printf("DoubleHash  | %16.0f | %15.4f | %14.2f | %13.2f\n",
           avg_sondagem_duplo, avg_sondagem_duplo / N, avg_colisoes_duplo, avg_max_duplo);

    double best_val = avg_sondagem_linear;
    const char *best_name = "Linear";
    double best_avg_per_insert = avg_sondagem_linear / N;
    double best_avg_coll = avg_colisoes_linear;

    if (avg_sondagem_quadratica < best_val - 1e-9) {
        best_val = avg_sondagem_quadratica;
        best_name = "Quadratico";
        best_avg_per_insert = avg_sondagem_quadratica / N;
        best_avg_coll = avg_colisoes_quadratica;
    }
    if (avg_sondagem_duplo < best_val - 1e-9) {
        best_val = avg_sondagem_duplo;
        best_name = "DoubleHash";
        best_avg_per_insert = avg_sondagem_duplo / N;
        best_avg_coll = avg_colisoes_duplo;
    }

    printf("\n>>> RESULTADO: para N = %d, o melhor metodo (menor avg_total_sondagem) foi: %s\n", N, best_name);
    printf("    avg_total_sondagem = %.0f, avg_sondagem/insercao = %.4f, avg_colisao = %.2f\n\n",
           best_val, best_avg_per_insert, best_avg_coll);

    free(pool);
    printf("\n");
}


int main(void) {
    srand((unsigned) time(NULL));

    int reps = 5; // Número de repetições para cada N
    int Ns[3] = {100, 1000, 10000};

    for (int i = 0; i < 3; i++)
        run_for_N(Ns[i], reps);

    return 0;
}
