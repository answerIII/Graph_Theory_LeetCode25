import java.util.*;

class Solution {
    public List<String> findAllRecipes(String[] recipes, List<List<String>> ingredients, String[] supplies) {
        int n = recipes.length;
        int[] indegree = new int[n];
        
        Map<String, Integer> recipeIndex = new HashMap<>();
        for (int i = 0; i < n; i++) {
            recipeIndex.put(recipes[i], i);
        }
        
        Map<String, List<Integer>> graph = new HashMap<>();
        for (int i = 0; i < n; i++) {
            for (String ing : ingredients.get(i)) {
                graph.computeIfAbsent(ing, k -> new ArrayList<>()).add(i);
                indegree[i]++;
            }
        }
        
        Queue<String> queue = new ArrayDeque<>();
        for (String supply : supplies) {
            queue.offer(supply);
        }
        
        List<String> result = new ArrayList<>();
        while (!queue.isEmpty()) {
            String item = queue.poll();
            List<Integer> dependentRecipes = graph.get(item);
            if (dependentRecipes == null) continue;
            for (int idx : dependentRecipes) {
                if (--indegree[idx] == 0) {
                    result.add(recipes[idx]);
                    queue.offer(recipes[idx]);
                }
            }
        }
        return result;
    }
}