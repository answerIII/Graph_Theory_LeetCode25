

/**
 * @param {number} n
 * @param {number[][]} edges
 * @return {boolean[]}
 */
var findAnswer = function(n, edges) {
    
    class MinHeap {
        
        constructor() {
            this.heap = [];
        }
        
        push(val) {
            
            this.heap.push(val);
            this._siftUp(this.heap.length - 1);
        }
        
        pop() {
            
            if (this.heap.length === 0) return null;
            if (this.heap.length === 1) return this.heap.pop();
            
            const result = this.heap[0];
            
            this.heap[0] = this.heap.pop();
            this._siftDown(0);
            
            return result;
        }
        
        _siftUp(index) {
            
            let parent = Math.floor((index - 1) / 2);
            
            while (index > 0 && this.heap[parent][0] > this.heap[index][0]) {
                
                [this.heap[parent], this.heap[index]] = [this.heap[index], this.heap[parent]];
                
                index = parent;
                parent = Math.floor((index - 1) / 2);
            }
        }
        
        _siftDown(index) {
            
            let minIndex = index;
            const len = this.heap.length;
            
            while (true) {
                
                const left = 2 * index + 1;
                const right = 2 * index + 2;
                
                if (left < len && this.heap[left][0] < this.heap[minIndex][0]) {
                    minIndex = left;
                }
                
                if (right < len && this.heap[right][0] < this.heap[minIndex][0]) {
                    minIndex = right;
                }
                
                if (minIndex === index) break;
                [this.heap[index], this.heap[minIndex]] = [this.heap[minIndex], this.heap[index]];
                index = minIndex;
            }
        }
        
        size() {
            return this.heap.length;
        }
    }
    
    // 1. Построим граф
    const graph = Array.from({ length: n }, () => []);
    const edgeMap = new Map(); // Для быстрого поиска индекса ребра
    
    for (let i = 0; i < edges.length; i++) {
        
        const [u, v, w] = edges[i];
        
        graph[u].push([v, w]);
        graph[v].push([u, w]);
        edgeMap.set(`${Math.min(u, v)},${Math.max(u, v)}`, i);
    }
    
    // 2. Дейкстра от 0: находим кратчайшие расстояния и DAG
    const dist = new Array(n).fill(Number.MAX_SAFE_INTEGER);
    const parents = Array.from({ length: n }, () => new Set());
    dist[0] = 0;

    const pq = new MinHeap();
    pq.push([0, 0]); // [distance, node]
    
    while (pq.size() > 0) {
        
        const [d, u] = pq.pop();
        
        if (d > dist[u]) continue;
        
        for (const [v, w] of graph[u]) {
            
            const newDist = dist[u] + w;
            
            if (newDist < dist[v]) {
                
                dist[v] = newDist;
                parents[v] = new Set([u]);
                pq.push([newDist, v]);
            } 
            else if (newDist === dist[v]) {
                parents[v].add(u);
            }
        }
    }
    
    // Если нет пути до n-1
    if (dist[n - 1] === Number.MAX_SAFE_INTEGER) {
        return new Array(edges.length).fill(false);
    }
    
    // 3. Построим DAG кратчайших путей с помощью BFS от n-1
    const answer = new Array(edges.length).fill(false);
    const visited = new Set();
    const queue = [n - 1];
    
    visited.add(n - 1);
    
    while (queue.length > 0) {
        
        const u = queue.shift();
        
        for (const v of parents[u]) {
            // Ребро (v, u) входит в кратчайший путь
            const edgeKey = `${Math.min(u, v)},${Math.max(u, v)}`;
            const edgeIdx = edgeMap.get(edgeKey);
            
            answer[edgeIdx] = true;
            
            if (!visited.has(v)) {
                
                visited.add(v);
                queue.push(v);
            }
        }
    }
    
    return answer;
};

module.exports = findAnswer;