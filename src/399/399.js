/**
 * @param {string[][]} equations
 * @param {number[]} values
 * @param {string[][]} queries
 * @return {number[]}
 */
var calcEquation = function(equations, values, queries) {
    // 1. Создаём граф как словарь смежности: ключ — вершина, значение — массив [сосед, вес]
    const graph = {};
    
    // 2. Заполняем граф на основе уравнений
    for (let i = 0; i < equations.length; i++) {
        const [a, b] = equations[i];
        const value = values[i];
        
        // Инициализируем вершины, если они ещё не существуют
        if (!graph[a]) graph[a] = [];
        if (!graph[b]) graph[b] = [];
        
        // Добавляем рёбра: a -> b с весом value, b -> a с весом 1/value
        graph[a].push([b, value]);
        graph[b].push([a, 1 / value]);
    }
    
    // 3. Функция DFS для поиска пути от start до end
    function dfs(start, end, visited) {
        // Если вершины нет в графе, возвращаем -1
        if (!graph[start] || !graph[end]) return -1.0;
        
        // Если достигли цели, возвращаем 1 (для случая start === end)
        if (start === end) return 1.0;
        
        visited.add(start);
        const neighbors = graph[start];
        
        // Обходим соседей
        for (const [next, weight] of neighbors) {
            if (!visited.has(next)) {
                const result = dfs(next, end, visited);
                // Если путь найден, возвращаем произведение весов
                if (result !== -1.0) {
                    return weight * result;
                }
            }
        }
        
        return -1.0;
    }
    
    // 4. Обрабатываем все запросы
    const results = [];
    for (const [c, d] of queries) {
        results.push(dfs(c, d, new Set()));
    }
    
    return results;
};

module.exports = calcEquation;