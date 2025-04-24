class Solution {
    public int[] countOfPairs(int n, int x, int y) {
        int[] res = new int[n];

        for (int i = 1; i <= n; ++i) {
            for (int j = i + 1; j <= n; ++j) {
                int direct = j - i;
                int viaXY = Math.abs(i - x) + 1 + Math.abs(j - y);
                int viaYX = Math.abs(i - y) + 1 + Math.abs(j - x);
                int d = Math.min(direct, Math.min(viaXY, viaYX));

                res[d - 1] += 2;
            }
        }
        return res;
    }
}