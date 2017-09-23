import java.io.*;
import java.util.*;
import java.security.*;

// --------------------------------------------------------
public class ConstrainedPermutationVis {
    static int maxN = 1000, minN = 10;
    int N;                          // numbers to permute
    int K;                          // number of given constraints
    int[][] constraints;
    String[] cArg;
    int[] raw_perm;                 // the permutation used for generation (for demo)
    // ---------------------------------------------------
    String generate(String seedStr) {
      try {
        SecureRandom rnd = SecureRandom.getInstance("SHA1PRNG");
        long seed = Long.parseLong(seedStr);
        rnd.setSeed(seed);

        N = rnd.nextInt(maxN - minN + 1) + minN;
        if (seed == 1)
            N = minN;
        else if (seed == 2)
            N = minN * 3;
        else if (seed == 3)
            N = maxN;

        // generate the constraints
        double pWrong = rnd.nextDouble() * 0.2 + 0.1;

        // generate a random permutation of numbers 0..N-1
        raw_perm = new int[N];
        for (int i = 0; i < N; ++i)
            raw_perm[i] = i;
        for (int i = 0; i < N - 1; ++i) {
            int j = rnd.nextInt(N-i)+i;
            int tmp = raw_perm[i];
            raw_perm[i] = raw_perm[j];
            raw_perm[j] = tmp;
        }

        // create constraints based on it (with most set correctly and some swapped)
        K = rnd.nextInt(N*(N-1)/4 - N + 1) + N;
        constraints = new int[K][2];
        HashSet<Integer> pairsUsed = new HashSet<>();
        for (int k = 0; k < K; ) {
            int i = rnd.nextInt(N);
            int j = rnd.nextInt(N);
            if (i == j)
                continue;
            Integer pair = new Integer(Math.max(i, j) * maxN + Math.min(i, j));
            if (pairsUsed.contains(pair))
                continue;
            // valid pair of indices - add a constraint
            pairsUsed.add(pair);
            // first make the constraint correct - make sure that p_i < p_j
            if (raw_perm[i] > raw_perm[j]) {
                int t = i;
                i = j;
                j = t;
            }
            // now make the constraint wrong with probability pWrong
            if (rnd.nextDouble() < pWrong) {
                int t = i;
                i = j;
                j = t;
            }
            // copy the constraint to the final array
            constraints[k][0] = i;
            constraints[k][1] = j;
            k++;
        }

        // convert to string argument
        cArg = new String[K];
        for (int i = 0; i < K; ++i)
            cArg[i] = constraints[i][0] + " " + constraints[i][1];

        // generate test representation
        StringBuffer sb = new StringBuffer();
        sb.append("N = " + N + '\n');
        sb.append("K = " + K + '\n');
        sb.append("Incorrect constraint probability = " + pWrong + '\n');
        if (seed < 3) {
            sb.append("Constraints:\n");
            for (int i = 0; i < K; ++i)
                sb.append(cArg[i]).append('\n');
        }
        return sb.toString();
      }
      catch (Exception e) {
        addFatalError("An exception occurred while generating test case.");
        e.printStackTrace();
        return "";
      }
    }
    // ---------------------------------------------------
    public double runTest(String seed) {
      try {
        String test = generate(seed);
        if (debug)
            System.out.println(test);

        // call user's solution and get return
        int[] perm;
        if (proc != null) {
            try {
                perm = permute(N, cArg);
            } catch (Exception e) {
                addFatalError("Failed to get result from permute.");
                return 0;
            }

            if (perm.length != N) {
                addFatalError("Your return must contain " + N + " elements, and it contained " + perm.length + ".");
                return 0;
            }

            // check that this is a valid permutation
            boolean[] used = new boolean[N];
            for (int i = 0; i < N; ++i) {
                if (perm[i] < 0 || perm[i] >= N) {
                    addFatalError("All elements of your return must be between 0 and " + (N-1) + ", and your return contained " + perm[i] + ".");
                    return 0;
                }
                if (used[perm[i]]) {
                    addFatalError("All elements of your return must be unique, and your return contained " + perm[i] + " twice.");
                    return 0;
                }
                used[perm[i]] = true;
            }
        }
        else {
            // use the original permutation (for scoring demo)
            perm = new int[N];
            for (int i = 0; i < N; ++i)
                perm[i] = raw_perm[i];
        }

        // calculate score
        int nOk = 0;
        for (int k = 0; k < K; ++k) {
            // decide whether constraint is satisfied
            int i = constraints[k][0];
            int j = constraints[k][1];
            if (perm[i] < perm[j])
                nOk++;
            else
                if (debug)
                    System.out.println("Bad constraint: " + perm[i] + "=p[" + i + "] > p[" + j + "]=" + perm[j]);
        }

        return nOk * 1.0 / K;
      }
      catch (Exception e) {
        addFatalError("An exception occurred while trying to process your program's results.");
        e.printStackTrace();
        return 0.0;
      }
    }
// ------------- visualization part ----------------------
    static String exec;
    static boolean debug;
    static Process proc;
    InputStream is;
    OutputStream os;
    BufferedReader br;
    // ---------------------------------------------------
    int[] permute(int N, String[] constr) throws IOException {
        // pass the params to the solution and get the return
        int i;
        StringBuffer sb = new StringBuffer();
        sb.append(N).append('\n');
        sb.append(constr.length).append('\n');
        for (i = 0; i < constr.length; ++i)
            sb.append(constr[i]).append('\n');
        os.write(sb.toString().getBytes());
        os.flush();

        // get the return - an array of integers
        int nRet = Integer.parseInt(br.readLine());
        int[] ret = new int[nRet];
        for (i = 0; i < nRet; ++i)
            ret[i] = Integer.parseInt(br.readLine());
        return ret;
    }
    // ---------------------------------------------------
    public ConstrainedPermutationVis(String seed) {
        // interface for runTest
        Thread thread = null;
        if (exec != null) {
            try {
                Runtime rt = Runtime.getRuntime();
                proc = rt.exec(exec);
                os = proc.getOutputStream();
                is = proc.getInputStream();
                br = new BufferedReader(new InputStreamReader(is));
                thread = new ErrorReader(proc.getErrorStream());
                thread.start();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        double s = runTest(seed);

        if (thread != null)
            try { thread.join(1000); } 
            catch (Exception e) { e.printStackTrace(); }

        System.out.println("Score = " + s);

        if (proc != null)
            try { proc.destroy(); }
            catch (Exception e) { e.printStackTrace(); }
    }
    // ---------------------------------------------------
    public static void main(String[] args) {
        String seed = "1";
        for (int i = 0; i<args.length; i++) {
            if (args[i].equals("-seed"))
                seed = args[++i];
            if (args[i].equals("-exec"))
                exec = args[++i];
            if (args[i].equals("-debug"))
                debug = true;
        }
        ConstrainedPermutationVis f = new ConstrainedPermutationVis(seed);
    }
    // ---------------------------------------------------
    void addFatalError(String message) {
        System.out.println(message);
    }
}

class ErrorReader extends Thread{
    InputStream error;
    public ErrorReader(InputStream is) {
        error = is;
    }
    public void run() {
        try {
            byte[] ch = new byte[50000];
            int read;
            while ((read = error.read(ch)) > 0)
            {   String s = new String(ch,0,read);
                System.out.print(s);
                System.out.flush();
            }
        } catch(Exception e) { }
    }
}
