function findAnswer(n: number, edges: number[][]): boolean[] {
  
  // MinHeap для Дейкстры, хранит индексы вершин
  class MinHeap {
    
    private heap: number[] = []; // Массив индексов вершин
    private distances: number[]; // Внешний массив расстояний

    constructor(distances: number[]) {
      this.distances = distances;
    }

    // Добавляет вершину в кучу
    push(vertex: number): void {
      this.heap.push(vertex);
      this._siftUp(this.heap.length - 1);
    }

    // Извлекает вершину с минимальным расстоянием
    pop(): number | null {
      if (this.heap.length === 0) return null;
      if (this.heap.length === 1) return this.heap.pop()!;
      const result = this.heap[0];
      this.heap[0] = this.heap.pop()!;
      this._siftDown(0);
      return result;
    }

    // Поднимает вершину вверх, чтобы сохранить свойство кучи
    private _siftUp(index: number): void {
      let parent = Math.floor((index - 1) / 2);
      while (index > 0 && this.distances[this.heap[parent]] > this.distances[this.heap[index]]) {
        [this.heap[parent], this.heap[index]] = [this.heap[index], this.heap[parent]];
        index = parent;
        parent = Math.floor((index - 1) / 2);
      }
    }

    // Опускает вершину вниз, чтобы сохранить свойство кучи
    private _siftDown(index: number): void {
      
      let minIndex = index;
      const len = this.heap.length;
      
      while (true) {
        
        const left = 2 * index + 1;
        const right = 2 * index + 2;
        
        if (left < len && this.distances[this.heap[left]] < this.distances[this.heap[minIndex]]) {
          minIndex = left;
        }
        
        if (right < len && this.distances[this.heap[right]] < this.distances[this.heap[minIndex]]) {
          minIndex = right;
        }
        
        if (minIndex === index) break;
        
        [this.heap[index], this.heap[minIndex]] = [this.heap[minIndex], this.heap[index]];
        index = minIndex;
      }
    }

    size(): number {
      return this.heap.length;
    }
  }

  // Строим граф: adj[u] хранит индексы рёбер, инцидентных вершине u
  const adj: number[][] = Array(n)
    .fill(0)
    .map(() => []);
  
  for (let i = 0; i < edges.length; i++) {
    
    const [a, b] = edges[i];
    adj[a].push(i);
    adj[b].push(i);
  }

  // Дейкстра от вершины 0: находим кратчайшие расстояния от 0
  const dist0: number[] = new Array(n).fill(Infinity);
  dist0[0] = 0;
  
  const heap = new MinHeap(dist0);
  heap.push(0);

  while (heap.size() > 0) {
    
    const u = heap.pop()!;
    
    for (const edgeIdx of adj[u]) {
      
      const v = edges[edgeIdx][0] === u ? edges[edgeIdx][1] : edges[edgeIdx][0];
      const w = edges[edgeIdx][2];
      
      if (dist0[v] > dist0[u] + w) {
        
        dist0[v] = dist0[u] + w;
        heap.push(v);
      }
    }
  }

  // Дейкстра от вершины n-1: находим кратчайшие расстояния до n-1
  const dist1: number[] = new Array(n).fill(Infinity);
  dist1[n - 1] = 0;
  
  const heap2 = new MinHeap(dist1);
  heap2.push(n - 1);

  while (heap2.size() > 0) {
    
    const u = heap2.pop()!;
    
    for (const edgeIdx of adj[u]) {
      
      const v = edges[edgeIdx][0] === u ? edges[edgeIdx][1] : edges[edgeIdx][0];
      const w = edges[edgeIdx][2];
      
      if (dist1[v] > dist1[u] + w) {
        
        dist1[v] = dist1[u] + w;
        heap2.push(v);
      }
    }
  }

  // Проверяем рёбра: лежит ли ребро на кратчайшем пути
  const total = dist0[n - 1];
  const ans: boolean[] = new Array(edges.length).fill(false);

  if (total === Infinity) {
    return ans;
  }

  for (let i = 0; i < edges.length; i++) {
    
    const [a, b, w] = edges[i];
    
    if (dist0[a] + w + dist1[b] === total || dist0[b] + w + dist1[a] === total) {
      ans[i] = true;
    }
  }

  return ans;
}