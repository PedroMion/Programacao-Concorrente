import java.util.concurrent.atomic.*;
import java.util.LinkedList;
import java.util.Random;
import java.lang.Math;

//-------------------------------------------------------------------------------
//Classe que define um pool de threads 
//!!! Documentar essa classe !!!
class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads;
    private final LinkedList<Runnable> queue;
    private boolean shutdown;

    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

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

class Recurso {
    //Recurso compartilhado para contar os primos
    private AtomicLong contador;

    //Construtor
    public Recurso(long init) { 
       this.contador = new AtomicLong(init); 
    }
  
    //Recupera o valor do contador
    public long recuperaContador() {
      return contador.get();
    }
  
    //Incrementa o valor do contador
    public void incrementaContador(){
      this.contador.getAndIncrement();  
    }
}
//-------------------------------------------------------------------------------

class VerificaPrimalidade implements Runnable {
    int numero;
    Recurso contador;

    public VerificaPrimalidade(int num, Recurso r) {
        this.numero = num;
        this.contador = r;
    }

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

    public void run() {
        if(this.ehPrimo(this.numero)) {
            this.contador.incrementaContador();
        }
    }
}
//-------------------------------------------------------------------------------

class ContaPrimos {
    private static final int NTHREADS = 4;
    private static final int LIMITESUPERIOR = 1000;
    private static final int LIMITEINFERIOR = 100;

    public static void main(String[] args) {
        Recurso contador = new Recurso(0);
        FilaTarefas pool = new FilaTarefas(NTHREADS);
        Random rand = new Random();

        // Gerando número aleatório entre 100 e 1000 (Ou qualquer limite superior e inferior desejado)
        int qntNumeros = (rand.nextInt(LIMITESUPERIOR - LIMITEINFERIOR) + LIMITEINFERIOR);

        for(int i = 1; i < qntNumeros; i++) {
            Runnable r = new VerificaPrimalidade(i, contador);
            pool.execute(r);
        }

        pool.shutdown();

        System.out.println("Quantidade de números primos entre 0 e " + qntNumeros + ": " + contador.recuperaContador());
    }

}