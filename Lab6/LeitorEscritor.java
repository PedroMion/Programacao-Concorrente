/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Leitores e escritores usando  Java */
/* -------------------------------------------------------------------*/

// Monitor
class LE {
  private int leit, escr, querEscrever;  
  
  // Construtor
  LE() { 
     this.leit = 0;
     this.escr = 0;
     this.querEscrever = 0; // Variável para escritores pedirem prioridade
  } 
  
  // Entrada para leitores
  public synchronized void EntraLeitor (int id) {
    try { 
      // Se tem alguém escrevendo ou querendo escrever, o leitor deve se bloquear
      while (this.escr > 0 || this.querEscrever > 0) {
      //if (this.escr > 0) {
         System.out.println ("le.leitorBloqueado("+id+")");
         wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.leit++; 
      System.out.println ("le.leitorLendo("+id+")");
    } catch (InterruptedException e) { }
  }
  
  // Saida para leitores
  public synchronized void SaiLeitor (int id) {
     this.leit--;
     if (this.leit == 0) 
           this.notify(); //libera escritor (caso exista escritor bloqueado)
     System.out.println ("le.leitorSaindo("+id+")");
  }
  
  // Entrada para escritores
  public synchronized void EntraEscritor (int id) {
    try { 
      // Se ele for o primeiro escritor, pede prioridade
      if(this.escr == 0 && this.querEscrever == 0) {
        this.querEscrever = 1;
        System.out.println("le.escritorPedePrioridade("+id+")");
      }
      while ((this.leit > 0) || (this.escr > 0)) {
         System.out.println ("le.escritorBloqueado("+id+")");
         wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.escr++; 
      System.out.println ("le.escritorEscrevendo("+id+")");
    } catch (InterruptedException e) { }
  }
  
  // Saida para escritores
  public synchronized void SaiEscritor (int id) {
     this.escr--;
     notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
     System.out.println ("le.escritorSaindo("+id+")");

     //Verifica se é o último, se sim, libera o querEscrever para que leitores possam ler
     if(this.escr == 0) {
      this.querEscrever = 0;
      System.out.println("le.ultimoEscritorSaindo("+id+")");
     }
  }
}

//--------------------------------------------------------
// Leitor
class Leitor extends Thread {
  int id; //identificador da thread
  LE monitor;//objeto monitor para coordenar a lógica de execução das threads

  // Construtor
  Leitor (int id, LE m) {
    this.id = id;
    this.monitor = m;
  }

  // Método executado pela thread
  public void run () {
      double j=777777777.7, i;
      for (;;) {
        this.monitor.EntraLeitor(this.id);
        for (i=0; i<100000000; i++) {j=j/2;} //...loop bobo para simbolizar o tempo de leitura
        this.monitor.SaiLeitor(this.id);
      }
  }
}

//--------------------------------------------------------
// Escritor
class Escritor extends Thread {
  int id; //identificador da thread
  LE monitor; //objeto monitor para coordenar a lógica de execução das threads

  // Construtor
  Escritor (int id, LE m) {
    this.id = id;
    this.monitor = m;
  }

  // Método executado pela thread
  public void run () {
      double j=777777777.7, i;
      for (;;) {
        this.monitor.EntraEscritor(this.id); 
        for (i=0; i<100000000; i++) {j=j/2;} //...loop bobo para simbolizar o tempo de escrita
        this.monitor.SaiEscritor(this.id); 
      }
  }
}

//--------------------------------------------------------
// Classe principal
class LeitorEscritor {
  static final int L = 4;
  static final int E = 3;

  public static void main (String[] args) {
    int i;
    LE monitor = new LE();            // Monitor (objeto compartilhado entre leitores e escritores)
    Leitor[] l = new Leitor[L];       // Threads leitores
    Escritor[] e = new Escritor[E];   // Threads escritores

    //inicia o log de saida
    System.out.println ("import verificaLE");
    System.out.println ("le = verificaLE.LE()");
    
    for (i=0; i<L; i++) {
       l[i] = new Leitor(i+1, monitor);
       l[i].start(); 
    }
    for (i=0; i<E; i++) {
       e[i] = new Escritor(i+1, monitor);
       e[i].start(); 
    }
  }
}
