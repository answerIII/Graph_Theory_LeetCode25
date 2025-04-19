/**
 * @param {number} n
 * @param {number[][]} dislikes
 * @return {boolean}
 */
var possibleBipartition = function(n, dislikes) {
    
    // 1. Создаём граф в виде списка смежности
    const graph = Array.from({ length: n + 1 }, () => []);
    
    for (const [a, b] of dislikes) {
        
        graph[a].push(b);
        graph[b].push(a); // Граф неориентированный
    }
    
    // 2. Массив для раскраски: 0 (не раскрашен), 1 (группа 1), -1 (группа 2)
    const colors = new Array(n + 1).fill(0);
    
    // 3. Проверяем каждую компоненту связности
    for (let node = 1; node <= n; node++) {
        
        if (colors[node] !== 0) continue; // Пропускаем уже раскрашенные вершины
        
        // BFS для раскраски
        const queue = [node];
        colors[node] = 1; // Начинаем с группы 1
        
        while (queue.length > 0) {
            
            const current = queue.shift();
            
            // Обрабатываем соседей
            for (const neighbor of graph[current]) {
                
                if (colors[neighbor] === 0) { // Не раскрашен
                    
                    colors[neighbor] = -colors[current]; // Противоположный цвет
                    queue.push(neighbor);
                } 
                else if (colors[neighbor] === colors[current]) { // Конфликт
                    return false;
                }
            }
        }
    }
    
    // Если конфликтов нет, граф двудольный
    return true;
};

module.exports = possibleBipartition;