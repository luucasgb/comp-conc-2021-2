/* Disciplina: Computacao Concorrente */
/* Prof: Silvana Rossetto */
/* Aluno: Lucas Guimarães Batista */
/* Laboratorio: 9 */
/* Codigo: Programa concorrente em Java que aplica o padrao leitores/escritores */
/* A aplicacao tera como elemento central uma variavel inteira que sera lida e alterada pelas threads */

class Monitor {
    private int leitores, escritores;

    // Construtor
    Monitor () {
        this.leitores = 0; // leitores lendo (0 ou mais)
        this.escritores = 0; // escritor escrevendo (0 ou 1)
    }

    // Entrada para leitores
    public synchronized void EntraLeitor (int id) {
        try {
            while (this.escritores > 0) {
                System.out.println ("le.leitorBloqueado(" + id + ")");
                wait();  // bloqueia pela condicao logica da aplicacao
            }
            this.leitores++;  // registra que ha mais um leitor lendo
            System.out.println ("le.leitorLendo(" + id + ")");
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
   
    // Saida para leitores
    public synchronized void SaiLeitor (int id) {
        this.leitores--; // registra que um leitor saiu

        if (this.leitores == 0)
            this.notify(); // libera escritor (caso exista escritor bloqueado)
        System.out.println ("le.leitorSaindo(" + id + ")");
    }

    // Entrada para escritores
    public synchronized void EntraEscritor (int id) {
        try {
            while ((this.leitores > 0) || (this.escritores > 0)) {
                System.out.println ("le.escritorBloqueado(" + id + ")");
                wait();  // bloqueia pela condicao logica da aplicacao
            }
            this.escritores++; // registra que ha um escritor escrevendo
            System.out.println ("le.escritorEscrevendo(" + id + ")");
        }

        catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    // Saida para escritores
    public synchronized void SaiEscritor (int id) {
        this.escritores--; // registra que o escritor saiu
        notifyAll(); // libera leitores e escritores (caso existam leitores ou escritores bloqueados)
        System.out.println ("le.escritorSaindo(" + id + ")");
    }
}

// Thread leitora
// Lê a variável compartilhada e imprime sua paridade
class Leitora extends Thread {
    private int id_thread;
    private Monitor monitor;

    // Construtor
    Leitora (int id_thread, Monitor monitor) {
        this.id_thread = id_thread;
        this.monitor = monitor;
    }

    // Teste de paridade
    private static boolean ehPar () {
        return Main.varGlobal % 2 == 0;
    }

    // Sobrescrevendo método run da classe base Thread
    @Override
    public void run () {
        this.monitor.EntraLeitor(this.id_thread);

        if (ehPar())
            System.out.println("Thread leitora " + this.id_thread + " leu o valor par " + Main.varGlobal);
        else
            System.out.println("Thread leitora " + this.id_thread + " leu o valor ímpar " + Main.varGlobal);

        this.monitor.SaiLeitor(this.id_thread);
    }
}

// Thread escritora
// Escreve na variável compartilha o seu identificador de thread
class Escritora extends Thread {
    private int id_thread;
    private Monitor monitor;

    // Construtor
    Escritora (int id_thread, Monitor monitor) {
        this.id_thread = id_thread;
        this.monitor = monitor;
    }

    // Sobrescrevendo método run da classe base Thread
    @Override
    public void run () {
        monitor.EntraEscritor(this.id_thread);

        Main.varGlobal = this.id_thread;
        System.out.println("Thread escritora " + this.id_thread + " escreveu o valor " + Main.varGlobal);

        monitor.SaiEscritor(this.id_thread);
    }
}

// Thread leitora/escritora
// Le variável e a imprime.
// Em sequencia faz um processamento e modifica a variável
class LeitoraEscritora extends Thread {
    private int id_thread;
    private Monitor monitor;

    LeitoraEscritora (int id_thread, Monitor monitor) {
        this.id_thread = id_thread;
        this.monitor = monitor;
    }

    // Sobrescrevendo método run da classe base Thread
    @Override
    public void run () {
        double j = 77777777777.7;
        this.monitor.EntraLeitor(this.id_thread);
        System.out.println("Thread leitora/escritora " + this.id_thread + " leu o valor " + Main.varGlobal);
        this.monitor.SaiLeitor(this.id_thread);
        this.monitor.EntraEscritor(this.id_thread);
        for (int i = 0; i < 100000000; i++) { j /= 2;}  // processamento ingênuo
        Main.varGlobal++;
        System.out.println("Thread leitora/escritora " + this.id_thread + " escreveu o valor " + Main.varGlobal);
        this.monitor.SaiEscritor(this.id_thread);
    }
}

// Classe principal
public class Main {
    public static int varGlobal = 0;
    public static final int THREAD_L_NUM = 4;
    public static final int THREAD_E_NUM = 4;
    public static final int THREAD_LE_NUM = 8;

    public static void main (String[] args) {
        Monitor monitor = new Monitor();
        Escritora[] E = new Escritora[THREAD_E_NUM];
        Leitora[] L = new Leitora[THREAD_L_NUM];
        LeitoraEscritora[] LE = new LeitoraEscritora[THREAD_LE_NUM];

        // Cria as threads escritoras
        for (int i = 0; i < THREAD_E_NUM; i++)
            E[i] = new Escritora(i, monitor);

        // Cria as threads leitoras
        for (int i = 0; i < THREAD_L_NUM; i++)
            L[i] = new Leitora(i, monitor);

        // Cria as threads leitoras/escritoras
        for (int i = 0; i < THREAD_LE_NUM; i++)
            LE[i] = new LeitoraEscritora(i, monitor);

        // Inicia as threads
        for (Escritora escritora : E)
            escritora.start();
        for (Leitora leitora : L)
            leitora.start();
        for (LeitoraEscritora leitoraEscritora : LE)
            leitoraEscritora.start();

        // Aguarda o termino das threads
        for (Escritora escritora : E)
            try { escritora.join(); } catch (InterruptedException e) { return; };

        for (Leitora leitora : L)
            try { leitora.join(); } catch (InterruptedException e) { return; };

        for (LeitoraEscritora leitoraEscritora : LE)
            try { leitoraEscritora.join(); } catch (InterruptedException e) { return; };
    }
}
