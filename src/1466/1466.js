/**
 * @param {number} n
 * @param {number[][]} connections
 * @return {number}
 */
var minReorder = function(n, connections) {
    
    // 1. Создаём граф: храним исходящие и входящие рёбра
    const graph = Array.from({ length: n }, () => ({ out: [], in: [] }));
    
    for (const [a, b] of connections) {
        
        graph[a].out.push(b); // Исходящее ребро: a -> b
        graph[b].in.push(a);  // Входящее ребро: b <- a
    }
    
    // 2. BFS от вершины 0
    let changes = 0;
    const visited = new Array(n).fill(false);
    const queue = [0];
    visited[0] = true;
    
    while (queue.length > 0) {
        
        const node = queue.shift();
        
        // Обрабатываем исходящие рёбра: их нужно переориентировать
        for (const next of graph[node].out) {
            
            if (!visited[next]) {
                
                changes++; // Переориентируем ребро node -> next на next -> node
                visited[next] = true;
                queue.push(next);
            }
        }
        
        // Обрабатываем входящие рёбра: они уже направлены к 0, переориентировать не нужно
        for (const next of graph[node].in) {
            
            if (!visited[next]) {
                
                visited[next] = true;
                queue.push(next);
            }
        }
    }
    
    return changes;
};

module.exports = minReorder;