
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

class Solution {

    private List<Integer>[] graph;
    private boolean[] visited;
    private int count;
    private int current_root;
    int[] roots;

    public int minReorder(int n, int[][] connections) {
        count = 0;
        graph = adjencyListAndInvert(connections, n);
        visited = new boolean[n];
        count = 0;
        List<Integer> roots_list = new ArrayList<>(); //сохраняем корни деревьев обхода
        roots = new int[n];
        Arrays.fill(roots, -1);
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                current_root = i;
                dfs(i);
                if (i != 0) {
                    roots_list.add(i);
                }

            }
        }
        //То что не соединено с 0 напрямую, соединяем через другие деревья
        while (!roots_list.isEmpty()) {
            Iterator<Integer> iterator = roots_list.iterator();
            while (iterator.hasNext()) {
                int r = iterator.next();
                boolean removed = false; // Флаг, показывающий, удалён ли элемент
                for (int v : graph[r]) {
                    if (roots[v] == 0) {
                        ++count;
                        roots[r] = 0;
                        if (!removed) {
                            iterator.remove(); // Удаляет элемент со значением 2
                            removed = true;
                        }
                        if (graph[r].size() > 1 && roots[graph[r].get(1)] == r) {
                            for (int s = 0; s < n; ++s) {
                                if (roots[s] == r) {
                                    roots[s] = 0;
                                }
                            }
                            break;
                        }

                    } else if (roots[v] != r) {
                        roots[v] = roots[r];
                        roots_list.remove(Integer.valueOf(v)); // Удаляет ПЕРВОЕ вхождение числа 2
                        if (graph[r].size() > 1) {

                            for (int s = 0; s < n; ++s) {
                                if (roots[s] == v) {
                                    roots[s] = roots[r];
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }

        return count;
    }

    static List<Integer>[] adjencyListAndInvert(int[][] edges, int n) {
        List<Integer>[] adj_list = new ArrayList[n];
        for (int i = 0; i < n; i++) {
            adj_list[i] = new ArrayList<>();
        }
        for (int[] edge : edges) {
            //inverting
            adj_list[edge[1]].add(edge[0]);
        }
        return adj_list;

    }

    void dfs(int node) {
        if (visited[node]) {
            return;
        }
        visited[node] = true;
        roots[node] = current_root;
        for (int u : graph[node]) {
            dfs(u);
        }
    }

}
