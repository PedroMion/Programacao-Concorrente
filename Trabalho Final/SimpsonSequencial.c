#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "timer.h"


typedef struct{
   double limiteInferior;
   double limiteSuperior;
   double tamanhoIntervalo;
   int quantidadeIteracoes;
} infosIntegracao;


double funcao(double x) {
   return (2*x*x*x) + (3*x*x);
}


double regraDeSimpsonSequencial(infosIntegracao infos) {
    double soma1 = 0, soma2 = 0, resultado;

    infos.tamanhoIntervalo = (infos.limiteSuperior-infos.limiteInferior) / infos.quantidadeIteracoes;
    double* valores = (double *) malloc(sizeof(double) * (infos.quantidadeIteracoes+1));

    for(int i = 0; i < (infos.quantidadeIteracoes+1); i++) {
        valores[i] = funcao(infos.limiteInferior + i*infos.tamanhoIntervalo);
    }

    for(int i = 1; i < ceil(infos.quantidadeIteracoes / 2); i++) {
        soma1 += valores[2 * i];
    }
    soma1 *= 2;

    for(int i = 1; i < ceil((infos.quantidadeIteracoes / 2) + 1); i++) {
        soma2 += valores[(2 * i) - 1];
    }
    soma2 *= 4;

    resultado = ((infos.tamanhoIntervalo/3) * (funcao(infos.limiteInferior) + funcao(infos.limiteSuperior) + soma1 + soma2));

    return resultado;
}


int main(int argc, char* argv[]) {
    infosIntegracao infos;
    double limiteSuperior, limiteInferior, resultado;
    double inicio, fim, delta;
    int quantidadeIteracoes;

    limiteInferior = 0;
    limiteSuperior = 8;
    quantidadeIteracoes = 15;

    infos.limiteInferior = limiteInferior;
    infos.limiteSuperior = limiteSuperior;
    infos.quantidadeIteracoes = quantidadeIteracoes;

    GET_TIME(inicio);
    resultado = regraDeSimpsonSequencial(infos);
    GET_TIME(fim);
    delta = fim - inicio;

    printf("Valor da função: %lf\n", resultado);
    printf("Tempo de execução sequencial: %lfs\n", delta);

   return 0;
}