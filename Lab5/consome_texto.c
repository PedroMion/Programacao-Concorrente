#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_LINHA 50

int nThreads;
char *nomeArquivo;
char *buffer;
int fimDoArquivo = 0;

sem_t slotVazio, slotCheio;
sem_t mutex;


//Função para as threads consumidoras
void *tarefa() {
    while(!fimDoArquivo) {
        //Verifica se possui algo no buffer para ser impresso
        sem_wait(&slotCheio);
        //Verifica se o lock está disponível
        sem_wait(&mutex);

        //Printa a linha
        printf("%s", buffer);

        //Libera o produtor para escrever
        sem_post(&slotVazio);
        //Devolve o lock
        sem_post(&mutex);
    }

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    pthread_t *tid;
    FILE *arquivo;

    //Inicializando variáveis de semáforo
    sem_init(&slotVazio, 0, 1);
    sem_init(&slotCheio, 0, 0);
    sem_init(&mutex, 0, 1);


    //Verificando se os argumentos foram passados na linha de comando
    if(argc < 3) {
        printf("Digite: %s <N de threads> <Nome do arquivo de texto>", argv[0]);
        return 1;
    }

    //Recebendo os argumentos
    nThreads = atoi(argv[1]);
    nomeArquivo = argv[2];

    //Alocando as variáveis
    tid = malloc(sizeof(pthread_t) * nThreads);
    buffer = malloc(sizeof(char) * MAX_LINHA);
    if(tid == NULL || buffer == NULL) { 
        printf("--ERRO: malloc()\n"); return 2;
    }

    //Criando as threads consumidoras
    for(int i = 0; i<nThreads; i++) {
        if(pthread_create(&tid[i], NULL, tarefa, NULL)) {
            printf("Erro na criacao do thread produtor\n");
            exit(1);
        }
    }

    //Abrindo arquivo
    arquivo = fopen(nomeArquivo, "r");

    // Verifica se o arquivo foi aberto com sucesso
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo");
        return 1;
    }

    // Lê o arquivo linha por linha
    while (!feof(arquivo)) {
        //Verifica se tem espaço vazio e escreve de maneira atômica
        sem_wait(&slotVazio);
        sem_wait(&mutex);

        //Escreve a próxima linha no buffer
        fgets(buffer, sizeof(buffer), arquivo);

        sem_post(&slotCheio);
        sem_post(&mutex);
    }

    //Espera um pouco e decreta o fim do arquivo, o que acarretará no fim das threads
    sleep(1);
    fimDoArquivo = 1;

    // Fecha o arquivo
    fclose(arquivo);

    free(tid);
    pthread_exit(NULL);
}