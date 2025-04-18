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
    
    // 3. Приоритетная очередь для Дейкстры: храним [prob, node]
    const pq = [[1, start_node]]; // [prob, node], начинаем с 1
    
    while (pq.length > 0) {
        
        // Сортируем очередь по убыванию вероятности
        pq.sort((a, b) => b[0] - a[0]); // Максимизируем вероятность
        const [prob, node] = pq.shift();
        
        // Если вероятность меньше уже найденной, пропускаем
        if (prob < probs[node]) continue;
        
        // Если достигли конечной вершины, возвращаем вероятность
        if (node === end_node) return prob;
        
        // Обрабатываем соседей
        for (const [next, edgeProb] of graph[node]) {
            
            const newProb = prob * edgeProb;
            
            if (newProb > probs[next]) {
                probs[next] = newProb;
                pq.push([newProb, next]);
            }
        }
    }
    
    // Если не достигли end_node, возвращаем 0
    return 0;
};

module.exports = maxProbability;