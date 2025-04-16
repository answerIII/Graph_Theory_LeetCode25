/**
 * @param {number[][]} graph
 * @return {number[][]}
 */
var allPathsSourceTarget = function(graph) {
    const n = graph.length;
    const result = [];
    
    // Функция DFS для построения путей
    function dfs(node, path) {
        
        // Если достигли целевой вершины (n-1), сохраняем путь
        if (node === n - 1) {
            result.push([...path]);
            return;
        }
        
        // Проходим по всем соседям текущей вершины
        for (const next of graph[node]) {
            path.push(next); // Добавляем следующую вершину в путь
            dfs(next, path); // Рекурсивный вызов
            path.pop(); // Удаляем вершину после возврата (backtracking)
        }
    }
    
    // Начинаем с вершины 0
    dfs(0, [0]);
    
    return result;
};

module.exports = allPathsSourceTarget;