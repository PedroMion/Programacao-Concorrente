import java.util.concurrent.atomic.*;
import java.util.LinkedList;
import java.util.Random;
import java.lang.Math;

//-------------------------------------------------------------------------------
// Classe que define um pool de threads 
class FilaTarefas {
    // Quantidade de threads (Recebida pela classe)
    private final int nThreads;
    // Vetor de Threads (pool)
    private final MyPoolThreads[] threads;
    // Lista encadeada com as tarefas a serem executadas pelo pool
    private final LinkedList<Runnable> queue;
    // Variável que controla o final do pool
    private boolean shutdown;

    public FilaTarefas(int nThreads) {
        // Criando e inicializando variáveis
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    // Função chamada para executar uma tarefa. Se não tiver desligada (shutdown false), adiciona na fila de tarefas
    // e notifca uma thread para deixar a espera
    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    // Função chamada para encerrar o pool, muda o valor da variável shutdown para true e libera todas as threads da espera
    // para que todas sejam encerradas. Ao final, espera todas terminarem (Ainda há a possibilidade de ter tarefas na fila)
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    // Classe que define as Threads, que ficam aguardando tarefas, quando são notificadas, recuperam o primeiro da fila e tentam executar
    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty() && shutdown) return;  
             r = (Runnable) queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------

// Classe para realizar a contagem de valores primos de forma mais organizada
class Recurso {
    // Recurso compartilhado para contar os primos (Atômico para não haver condição de corrida)
    private AtomicLong contador;

    // Construtor
    public Recurso(long init) { 
       this.contador = new AtomicLong(init); 
    }
  
    // Recupera o valor do contador
    public long recuperaContador() {
      return contador.get();
    }
  
    // Incrementa o valor do contador
    public void incrementaContador(){
      this.contador.getAndIncrement();  
    }
}
//-------------------------------------------------------------------------------

// Classe com a lógica que será executada pelas Threads
class VerificaPrimalidade implements Runnable {
    // Definindo o número a ser verificado se é primo e o contador
    int numero;
    Recurso contador;

    // Construtor da classe
    public VerificaPrimalidade(int num, Recurso r) {
        this.numero = num;
        this.contador = r;
    }

    // Função da professora para verificar primalidade, com ajustes para se encaixar melhor ao Java
    private boolean ehPrimo(int n) {
        int i;

        if (n<=1) return false;
        if (n==2) return true;
        if (n%2==0) return false;

        for (i=3; i<Math.sqrt(n)+1; i+=2) {
            if(n%i==0) return false;
        }

        return true;
    }

    // Método executado pelas threads, se o numero for primo, incrementa o contador
    public void run() {
        if(this.ehPrimo(this.numero)) {
            this.contador.incrementaContador();
        }
    }
}
//-------------------------------------------------------------------------------

// Classe principal da aplicação
class ContaPrimos {
    // Constantes que definem número de threads e os limites superior e inferior do número aleatório gerado pela main
    // Basta mudar essas constantes para testar com valores diferentes
    private static final int NTHREADS = 4;
    private static final int LIMITESUPERIOR = 1000;
    private static final int LIMITEINFERIOR = 100;

    public static void main(String[] args) {
        Recurso contador = new Recurso(0);
        FilaTarefas pool = new FilaTarefas(NTHREADS);
        Random rand = new Random();

        // Gerando número aleatório entre 100 e 1000 (Ou qualquer limite superior e inferior desejado)
        int qntNumeros = (rand.nextInt(LIMITESUPERIOR - LIMITEINFERIOR) + LIMITEINFERIOR);

        // Executando a tarefa de verificar se é primo através do pool de threads
        for(int i = 1; i < qntNumeros; i++) {
            Runnable r = new VerificaPrimalidade(i, contador);
            pool.execute(r);
        }

        // Desabilita o pool
        pool.shutdown();

        System.out.println("Quantidade de números primos entre 0 e " + qntNumeros + ": " + contador.recuperaContador());
    }

}