#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include "timer.h"


long long int N;
int N_THREADS;
int qntNumerosPrimos = 0;
pthread_mutex_t mutex;


typedef struct{
  int id;
} tArgs;


int ehPrimo(long long int n) {
   int i;
   if (n<=1) return 0;
   if (n==2) return 1;
   if (n%2==0) return 0;


   for (i=3; i<sqrt(n)+1; i+=2) {
       if(n%i==0) return 0;
   }


   return 1;
}


void * tarefa(void *arg) {
  tArgs *args = (tArgs*) arg;
  int qntNumerosPrimosLocal = 0;


  for(long long int i=args->id; i < N; i += N_THREADS) {
     qntNumerosPrimosLocal += ehPrimo(i);
  }


   pthread_mutex_lock(&mutex);
   qntNumerosPrimos += qntNumerosPrimosLocal;
   pthread_mutex_unlock(&mutex);


  pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
  pthread_t *tid;
  tArgs *args;
  double inicio, fim, delta;


  if(argc<3) {
     printf("Digite: %s <valor de n> <numero de threads>\n", argv[0]);
     return 1;
  }
  N = atoi(argv[1]);
  N_THREADS = atoi(argv[2]);
  if (N_THREADS > N) N_THREADS = N;


  GET_TIME(inicio);
 
  tid = (pthread_t*) malloc(sizeof(pthread_t)*N_THREADS);
  if(tid==NULL) {puts("ERRO--malloc"); return 2;}
  args = (tArgs*) malloc(sizeof(tArgs)*N_THREADS);
  if(args==NULL) {puts("ERRO--malloc"); return 2;}
 
  pthread_mutex_init(&mutex, NULL);


  for(int i=0; i<N_THREADS; i++) {
     (args+i)->id = i+1;


     if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
        puts("ERRO--pthread_create"); return 3;
     }
  }


  for(int i=0; i<N_THREADS; i++) {
     pthread_join(*(tid+i), NULL);
  }


  pthread_mutex_destroy(&mutex);


  GET_TIME(fim)  
  delta = fim - inicio;
  printf("Tempo execucao concorrente: %lf\n", delta);


  free(args);
  free(tid);


  printf("Quantidade de numeros primos entre 1 e %lld: %d", N, qntNumerosPrimos);


  return 0;
}
