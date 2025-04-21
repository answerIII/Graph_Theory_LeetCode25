import java.util.*;

class Solution {
    public int minOperations(int n, int m) {
        if (n == m) {
            return isPrime(n) ? -1 : n;  
        }
        if (isPrime(n) || isPrime(m)) {
            return -1;
        }
        
        int D = String.valueOf(n).length();
        int MAX = (int) Math.pow(10, D);
        boolean[] prime = sieve(10000);
        
        long[] dist = new long[10000];
        Arrays.fill(dist, Long.MAX_VALUE);
        dist[n] = n;
        PriorityQueue<long[]> pq = new PriorityQueue<>(Comparator.comparingLong(a -> a[0]));
        pq.offer(new long[]{n, n});
        
        while (!pq.isEmpty()) {
            long[] cur = pq.poll();
            long cost = cur[0];
            int x = (int) cur[1];
            if (x == m) return (int) cost;
            if (cost > dist[x]) continue;
            
            int[] digits = new int[D];
            int tmp = x;
            for (int i = D - 1; i >= 0; --i) {
                digits[i] = tmp % 10;
                tmp /= 10;
            }
            
            for (int i = 0; i < D; ++i) {
                int d = digits[i];
                if (d < 9) {
                    int y = buildNeighbor(digits, i, d + 1);
                    if (y >= MAX/10 && !prime[y]) {
                        long nc = cost + y;
                        if (nc < dist[y]) {
                            dist[y] = nc;
                            pq.offer(new long[]{nc, y});
                        }
                    }
                }
                if (d > 0) {
                    if (!(i == 0 && d - 1 == 0)) {
                        int y = buildNeighbor(digits, i, d - 1);
                        if (y >= MAX/10 && !prime[y]) {
                            long nc = cost + y;
                            if (nc < dist[y]) {
                                dist[y] = nc;
                                pq.offer(new long[]{nc, y});
                            }
                        }
                    }
                }
            }
        }
        
        return -1;
    }
    
    private int buildNeighbor(int[] digits, int i, int val) {
        int res = 0;
        for (int j = 0; j < digits.length; ++j) {
            res = res * 10 + (j == i ? val : digits[j]);
        }
        return res;
    }
    
    private static boolean isPrime(int x) {
        if (x < 2) return false;
        int limit = (int)Math.sqrt(x);
        for (int i = 2; i <= limit; i++) {
            if (x % i == 0) return false;
        }
        return true;
    }
    
    private boolean[] sieve(int n) {
        boolean[] prime = new boolean[n];
        Arrays.fill(prime, true);
        prime[0] = prime[1] = false;
        for (int i = 2; i * i < n; i++) {
            if (prime[i]) {
                for (int j = i * i; j < n; j += i) {
                    prime[j] = false;
                }
            }
        }
        return prime;
    }
}