import java.util.Random;

class Sum {

    // Fields
    private int sum;

    // Constructor
    public Sum ()
    {
        this.sum = 0;
    }

    // Getter
    public synchronized int getSum () {
        return this.sum;
    }

    // Setter
    public synchronized void setSum (int i)
    {
        this.sum = i;
    }

    // Setter
    public synchronized void addToSum (int i) {
        this.sum += i;
    }
}

class ConcSum extends Thread {
    private int id_thread;
    private int nthreads;
    private int[] values;
    private Sum sum;

    // Constructor
    public ConcSum(int id_thread, int nthreads, int[] values, Sum sum) {
        this.id_thread = id_thread;
        this.nthreads = nthreads;
        this.values = values;
        this.sum = sum;
    }

    // Run method from base class Thread
    public void run () {
        for (int i = this.id_thread; i < this.values.length; i += this.nthreads)
            this.sum.addToSum(this.values[i]);
    }
}

public class Main {
    static final int LENGTH = 10000; 
    static final int THREAD_NUM = 10;

    // Compares a value to the sumprivate  of an int array
    private static boolean checkSum(Sum sum, int[] values) {
        int seqSum = 0;
        int concSum = sum.getSum();
        for (int i : values) 
            seqSum += i;
        System.out.printf("Soma sequencial: %d\n", seqSum);
        System.out.printf("Soma concorrente: %d\n", concSum);
        return seqSum == concSum;
    }


    public static void main (String[] args) {
        int[] values = new int[LENGTH];
        Thread[] threads = new Thread[THREAD_NUM];

        // Fills array
        Random random = new Random();
        for (int i = 0; i < LENGTH; i++)
            values[i] = 1 + random.nextInt(1000);

        Sum sum = new Sum();

        // Creating threads
        for (int i = 0; i < threads.length; i++)
            threads[i] = new ConcSum(i, THREAD_NUM, values, sum);

        // Starting threads
        for (Thread thread : threads) thread.start();

        // Waits for threads to finish
        for (Thread thread : threads)
            try { thread.join(); } catch (InterruptedException e) { return; };

        // Checks for correctness
        if (checkSum(sum, values)) 
        {
            System.out.println("Soma calculada corretamente");
            return;
        }
        System.out.println("--ERRO: Soma não foi calculada corretamente");
    }
}
