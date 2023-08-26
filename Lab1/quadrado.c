#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <time.h>

#define NTHREADS  8
#define TAMANHO_VETOR  10000

typedef struct {
    int indexThread, tamanhoVetor, nThreads;
    int *vetor;
} t_Args;

void *elevaAoQuadrado (void *arg) {
    t_Args *args = (t_Args *) arg;
    int i = args->indexThread;
    int tamanho = args->tamanhoVetor;
    int nThreads = args->nThreads;
    int *vetor = args->vetor;

    for( ; i<tamanho; i += nThreads) {
        vetor[i] = vetor[i] * vetor[i];
    }

    free(args); 

    printf("Fim da thread: %d\n", i%nThreads);
    pthread_exit(NULL);
}

int *defineVetor(int tamanho) {
    time_t t;
    int *vetor = malloc(sizeof(int)*tamanho);
    if(vetor == NULL) {
        printf("--ERRO: malloc()\n"); exit(-1);
    }

    srand((unsigned) time(&t));
    for(int i=0; i<tamanho; i++){
        vetor[i] = rand() % 5000;
    }

    return vetor;
}

int verificaVetor(int *vetorNovo, int *vetorOriginal, int tamanho) {
    for(int i=0; i<tamanho; i++) {
        if(vetorNovo[i] != (vetorOriginal[i]*vetorOriginal[i])) return 1;
    }

    return 0;
}

int main() {
    pthread_t tid_sistema[NTHREADS];
    t_Args *args;
    int *vetor, *vetorOriginal;

    vetor = defineVetor(TAMANHO_VETOR);
    vetorOriginal = malloc(sizeof(int) * TAMANHO_VETOR);

    for(int i=0; i<TAMANHO_VETOR; i++) {
        vetorOriginal[i] = vetor[i];
    }

    for(int i=0; i<NTHREADS; i++) {
        args = malloc(sizeof(t_Args));
        if (args == NULL) {
            printf("--ERRO: malloc()\n"); exit(-1);
        }
        args->indexThread = i;
        args->vetor = vetor;
        args->tamanhoVetor = TAMANHO_VETOR;
        args->nThreads = NTHREADS;

        if (pthread_create(&tid_sistema[i], NULL, elevaAoQuadrado, (void*) args)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    for (int i=0; i<NTHREADS; i++) {
        if (pthread_join(tid_sistema[i], NULL)) {
                printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    }

    if(verificaVetor(vetor, vetorOriginal, TAMANHO_VETOR) == 0) {
        printf("Valores corretos\n");
    }
    else {
        printf("Algo deu errado\n");
    }

    for(int i=0; i<10; i++) {
        printf("Valor original %d: %d, valor ao quadrado: %d\n", i, vetorOriginal[i], vetor[i]);
    }

    pthread_exit(NULL);
}