import java.util.*;

class Solution {
    public List<String> findAllRecipes(String[] recipes, List<List<String>> ingredients, String[] supplies) {
        Map<String, List<String>> graph = new HashMap<>();
        Map<String, Integer> indegree = new HashMap<>();
        
        for (String recipe : recipes) {
            indegree.put(recipe, 0);
        }
        
        for (int i = 0; i < recipes.length; i++) {
            for (String ing : ingredients.get(i)) {
                graph.computeIfAbsent(ing, k -> new ArrayList<>()).add(recipes[i]);
                indegree.put(recipes[i], indegree.get(recipes[i]) + 1);
            }
        }
        
        Queue<String> queue = new LinkedList<>();
        for (String supply : supplies) {
            queue.offer(supply);
        }
        
        List<String> result = new ArrayList<>();
        
        while (!queue.isEmpty()) {
            String item = queue.poll();
            if (!graph.containsKey(item)) {
                continue;
            }
            for (String recipe : graph.get(item)) {
                indegree.put(recipe, indegree.get(recipe) - 1);

                if (indegree.get(recipe) == 0) {
                    result.add(recipe);
                    queue.offer(recipe);
                }
            }
        }
        
        return result;
    }
}