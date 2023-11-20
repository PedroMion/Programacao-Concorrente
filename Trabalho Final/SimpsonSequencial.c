#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct{
   double limiteInferior;
   double limiteSuperior;
   double tamanhoIntervalo;
   int quantidadeIteracoes;
} infosIntegracao;


double funcao(double x) {
   return 2*x;
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
    int quantidadeIteracoes;

    limiteInferior = 10;
    limiteSuperior = 20;
    quantidadeIteracoes = 100;

    infos.limiteInferior = limiteInferior;
    infos.limiteSuperior = limiteSuperior;
    infos.quantidadeIteracoes = quantidadeIteracoes;

    resultado = regraDeSimpsonSequencial(infos);

    printf("Valor da função: %lf\n", resultado);

   return 0;
}