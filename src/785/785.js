/**
 * @param {number[][]} graph
 * @return {boolean}
 */
var isBipartite = function(graph) {
    
    const n = graph.length;
    // Массив цветов: -1 — не покрашено, 0 — первый цвет, 1 — второй цвет
    const colors = new Array(n).fill(-1);
    
    // Проходим по всем вершинам, так как граф может быть несвязным
    for (let start = 0; start < n; start++) {
        
        if (colors[start] !== -1) continue; // Пропускаем уже покрашенные вершины
        
        // Начинаем BFS с вершины start, красим её в цвет 0
        colors[start] = 0;
        const queue = [start];
        
        while (queue.length > 0) {
            
            const node = queue.shift();
            const currentColor = colors[node];
            const nextColor = 1 - currentColor; // Противоположный цвет (0 -> 1, 1 -> 0)
            
            // Обрабатываем всех соседей текущей вершины
            for (const neighbor of graph[node]) {
                
                if (colors[neighbor] === -1) {
                    // Если сосед не покрашен, красим его в противоположный цвет
                    colors[neighbor] = nextColor;
                    queue.push(neighbor);
                } 
                else if (colors[neighbor] === currentColor) {
                    // Если сосед уже покрашен и цвет совпадает с текущим, граф не двудольный
                    return false;
                }
                // Если colors[neighbor] === nextColor, всё ок, продолжаем
            }
        }
    }
    
    return true;
};

module.exports = isBipartite;