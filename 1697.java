
import java.util.*;

class Solution {

    private List<int[]>[] graph;
    private int n;

    static int[] parent;

    static int find(int v) {
        if (parent[v] == v) {
            return v;
        }
        return parent[v] = find(parent[v]);
    }

    static void union(int a, int b) {
        parent[find(a)] = find(b);
    }
    int edgesUsed = 0;

    public void kruskal(int[][] edges) {

        int mstWeight = 0;

        for (int[] edge : edges) {
            int u = edge[0], v = edge[1], weight = edge[2];
            if (find(u) != find(v)) { // Если вершины в разных компонентах
                union(u, v);
                mstWeight += weight;
                edgesUsed++;
                if (edgesUsed == n - 1) {
                    break; // Остовное дерево построено

                }
            }
        }
        // return mstWeight; // Вес минимального остовного дерева
    }

    public boolean[] distanceLimitedPathsExist(int n, int[][] edgeList, int[][] queries) {
        this.n = n;
        boolean[] result = new boolean[queries.length];
        for(int i=0; i<queries.length; ++i){
            queries[i]= new int[]{queries[i][0], queries[i][1], queries[i][2], i};
        }
        Arrays.sort(queries, Comparator.comparingInt(a -> a[2]));
        Arrays.sort(edgeList, Comparator.comparingInt(a -> a[2]));

        int low = 0;
        int high = 0;
        int edgeList_ind = 0;
        parent = new int[n];
        for (int i = 0; i < n; i++) {
            parent[i] = i; // Инициализируем DSU
        }
        for (int i = 0; i < queries.length; ++i) {
            if (queries[i][2] > high) {
                low = high;
                high = queries[i][2];
                int leftIndex = Arrays.binarySearch(edgeList, new int[]{0, 0, low},
                        (a, b) -> Integer.compare(a[2], b[2]));
                if (leftIndex < 0) {
                    leftIndex = -leftIndex - 1; // Корректировка, если точного совпадения нет
                }

// Находим конечный индекс (первый элемент, где edgeList[i][2] >= high)
                int rightIndex = Arrays.binarySearch(edgeList, new int[]{0, 0, high},
                        (a, b) -> Integer.compare(a[2], b[2]));
                if (rightIndex < 0) {
                    rightIndex = -rightIndex - 1;
                }

// Получаем подмассив (от leftIndex до rightIndex-1)
                int[][] subarray = Arrays.copyOfRange(edgeList, leftIndex, rightIndex-1);
                kruskal(subarray);

            }
            //result[i] = prim(queries[i][0], queries[i][1], queries[i][2]);
                result[queries[i][3]] = find(queries[i][0]) == find(queries[i][1]);
        }
        return result;

    }

   
}