#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_LINHA 100
#define TAM_BUFFER 5

int nThreads;
char **buffer;

sem_t slotVazio, slotCheio;
sem_t mutex;


void produtor(char *string) {
    static int in = 0;

    //Verifica se existem espaços vazios
    sem_wait(&slotVazio);
    //Verifica se o lock está disponível
    sem_wait(&mutex);

    //Aloca memória para a próxima linha no buffer
    buffer[in] = (char *)malloc(sizeof(char) * MAX_LINHA);
    //Copia a linha para o buffer
    strcpy(buffer[in], string);
    //Incrementa in
    in = (in + 1) % TAM_BUFFER;

    //Dá o sinal que mais uma posição ficou cheia
    sem_post(&slotCheio);
    //Devolve o mutex
    sem_post(&mutex);

}

void consumidor() {
    static int in = 0;
    char *linha;

    //Verifica se possui algo no buffer para ser impresso
    sem_wait(&slotCheio);
    //Verifica se o lock está disponível
    sem_wait(&mutex);

    //Le a linha
    linha = buffer[in];
    printf("%s", linha);
    //Incrementa in
    in = (in + 1) % TAM_BUFFER;

    //Libera o produtor para escrever
    sem_post(&slotVazio);
    //Devolve o lock
    sem_post(&mutex);
}

//Função para as threads consumidoras
void *tarefa() {
    while(1) {
        consumidor();
    }

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    pthread_t *tid;
    FILE *arquivo;
    char *linha;

    //Inicializando variáveis de semáforo, começamos com tamanho do buffer livres e nenhum cheio
    sem_init(&slotVazio, 0, TAM_BUFFER);
    sem_init(&slotCheio, 0, 0);
    sem_init(&mutex, 0, 1);


    //Verificando se os argumentos foram passados na linha de comando
    if(argc < 3) {
        printf("Digite: %s <N de threads> <Nome do arquivo de texto>", argv[0]);
        return 1;
    }

    //Recebendo os argumentos
    nThreads = atoi(argv[1]);

    //Abrindo arquivo
    arquivo = fopen(argv[2], "r");

    // Verifica se o arquivo foi aberto com sucesso
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo");
        return 1;
    }

    //Alocando as variáveis
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nThreads);
    linha = (char* )malloc(sizeof(char) * MAX_LINHA);
    buffer = (char**) malloc(sizeof(char *) * TAM_BUFFER);

    //Verificando erros na alocação
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

    // Lê o arquivo linha por linha
    while (fgets(linha, MAX_LINHA, arquivo)) {
        produtor(linha);
    }

    // Fecha o arquivo
    fclose(arquivo);

    pthread_exit(NULL);
}