#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

int N_THREADS = 3;
double resultado;
pthread_mutex_t mutex;


typedef struct{
   double limiteInferior;
   double limiteSuperior;
   int quantidadeIteracoes;
   double tamanhoIntervalo;
} infosIntegracao;


double funcao(double x) {
   return (2*x*x*x) + (3*x*x);
}


void * regraDeSimpsonConcorrente(void *arg) {
    infosIntegracao* infos = (infosIntegracao *) arg;
    double soma1 = 0, soma2 = 0, resultadoLocal;

    infos->tamanhoIntervalo = (infos->limiteSuperior-infos->limiteInferior) / infos->quantidadeIteracoes;
    double* valores = (double *) malloc(sizeof(double) * (infos->quantidadeIteracoes+1));

    for(int i = 0; i < (infos->quantidadeIteracoes+1); i++) {
        valores[i] = funcao(infos->limiteInferior + i*infos->tamanhoIntervalo);
    }

    for(int i = 1; i < ceil(infos->quantidadeIteracoes / 2); i++) {
        soma1 += valores[2 * i];
    }
    soma1 *= 2;

    for(int i = 1; i < ceil((infos->quantidadeIteracoes / 2) + 1); i++) {
        soma2 += valores[(2 * i) - 1];
    }
    soma2 *= 4;

    resultadoLocal = ((infos->tamanhoIntervalo/3) * (funcao(infos->limiteInferior) + funcao(infos->limiteSuperior) + soma1 + soma2));

    pthread_mutex_lock(&mutex);
    resultado += resultadoLocal;
    pthread_mutex_unlock(&mutex);
    printf("Resultado local para a=%lf, b=%lf e n=%d: %lf\n", infos->limiteInferior, infos->limiteSuperior, infos->quantidadeIteracoes, resultadoLocal);

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    pthread_t *tid;
    infosIntegracao* infos;
    double limiteSuperior, limiteInferior, diferenca;
    double inicio, fim, delta;
    int quantidadeIteracoes;

    GET_TIME(inicio);

    tid = (pthread_t*) malloc(sizeof(pthread_t)*N_THREADS);
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}

    infos = (infosIntegracao*) malloc(sizeof(infosIntegracao)*N_THREADS);
    if(infos==NULL) {puts("ERRO--malloc"); return 2;}

    pthread_mutex_init(&mutex, NULL);

    limiteInferior = 0;
    limiteSuperior = 50;
    quantidadeIteracoes = 100;

    resultado = 0;
    diferenca = (limiteSuperior - limiteInferior) / N_THREADS;
    for(int i = 0; i < N_THREADS; i++) {
        (infos + i)->limiteInferior = ceil(limiteInferior + (i * diferenca));
        (infos + i)->limiteSuperior = ceil(limiteInferior + ((i+1) * diferenca));
        (infos + i)->quantidadeIteracoes = 2*((infos + i)->limiteSuperior - (infos + i)->limiteInferior);

        if(pthread_create(tid+i, NULL, regraDeSimpsonConcorrente, (void*) (infos + i))){
            puts("ERRO--pthread_create"); return 3;
         }
    }

    for(int i = 0; i < N_THREADS; i++) {
        pthread_join(*(tid+i), NULL);
    }

    GET_TIME(fim);
    delta = fim - inicio;
    pthread_mutex_destroy(&mutex);

    free(tid);

    printf("Valor da função: %lf\n", resultado);
    printf("Tempo de execução concorrente: %lfs\n", delta);

   return 0;
}