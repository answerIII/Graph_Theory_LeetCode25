/**
 * @param {number} n
 * @param {number[][]} edges
 * @param {number[]} succProb
 * @param {number} start_node
 * @param {number} end_node
 * @return {number}
 */
var maxProbability = function(n, edges, succProb, start_node, end_node) {
    
    // 1. Создаём граф в виде списка смежности
    const graph = Array.from({ length: n }, () => []);
    
    for (let i = 0; i < edges.length; i++) {
        
        const [a, b] = edges[i];
        const prob = succProb[i];
        
        graph[a].push([b, prob]);
        graph[b].push([a, prob]); // Граф неориентированный
    }
    
    // 2. Массив для хранения максимальной вероятности до каждой вершины
    const probs = new Array(n).fill(0);
    probs[start_node] = 1; // Вероятность в начальной вершине — 1
    
    // 3. BFS с очередью
    const queue = [start_node];
    
    while (queue.length > 0) {
        const node = queue.shift();
        
        // Обрабатываем соседей
        for (const [next, edgeProb] of graph[node]) {
            
            const newProb = probs[node] * edgeProb;
            
            if (newProb > probs[next]) {
                
                probs[next] = newProb;
                queue.push(next); // Добавляем в очередь для дальнейшей обработки
            }
        }
    }
    
    // Возвращаем вероятность для end_node
    return probs[end_node];
};

module.exports = maxProbability;