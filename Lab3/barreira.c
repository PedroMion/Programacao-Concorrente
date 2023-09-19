#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int N_THREADS;
int aux;
pthread_mutex_t mutex;
pthread_cond_t x_cond;

typedef struct{
 int idThread;
} tArgs;

void barreira(int idThread) {
   // Início do trecho atômico
  pthread_mutex_lock(&mutex);
  // Incrementa a variável que controla quantas threads já alcançaram a barreira
  aux += 1;
  if(aux != N_THREADS) {
   // Se não for a última, se bloqueia
    pthread_cond_wait(&x_cond, &mutex);
  }
  else {
   // Se for a última, libera as demais
    pthread_cond_broadcast(&x_cond);
    aux = 0;
  }
   // Fim do trecho atômico
 pthread_mutex_unlock(&mutex);
}

void * tarefa(void *arg) {
 tArgs *args = (tArgs*) arg;

 printf("Olá da thread %d\n", args->idThread);
 barreira(args->idThread);

 printf("Que dia bonito %d\n", args->idThread);
 barreira(args->idThread);

 printf("Até breve da thread %d\n", args->idThread);
 barreira(args->idThread);

 pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
 pthread_t *tid;
 tArgs *args;

 if(argc<2) {
    printf("Digite: %s <numero de threads>\n", argv[0]);
    return 1;
 }

 N_THREADS = atoi(argv[1]);

 tid = (pthread_t*) malloc(sizeof(pthread_t)*N_THREADS);
 if(tid==NULL) {puts("ERRO--malloc"); return 2;}

 args = (tArgs*) malloc(sizeof(tArgs)*N_THREADS);
 if(args==NULL) {puts("ERRO--malloc"); return 2;}

 pthread_mutex_init(&mutex, NULL);
 pthread_cond_init(&x_cond, NULL);

 aux = 0;
 for(int i=0; i<N_THREADS; i += 1) {
    (args+i)->idThread = i;

    if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
       puts("ERRO--pthread_create"); return 3;
    }
 }

 for(int i=0; i<N_THREADS; i++) {
    pthread_join(*(tid+i), NULL);
 }

 pthread_mutex_destroy(&mutex);

 free(args);
 free(tid);

 return 0;
}
