package _882

import "container/heap"

type pqItem struct {
	moves  int
	idNode int
}

type minHeap []pqItem

func (h minHeap) Len() int           { return len(h) }
func (h minHeap) Less(i, j int) bool { return h[i].moves > h[j].moves }
func (h minHeap) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }

func (h *minHeap) Push(x interface{}) {
	*h = append(*h, x.(pqItem))
}

func (h *minHeap) Pop() interface{} {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[:n-1]
	return x
}

func reachableNodes(edges [][]int, maxMoves int, n int) int {
	graph := make(map[int]map[int]int)
	for i := 0; i < n; i++ {
		graph[i] = make(map[int]int)
	}
	for _, edge := range edges {
		graph[edge[0]][edge[1]] = edge[2]
		graph[edge[1]][edge[0]] = edge[2]
	}

	h := &minHeap{}
	heap.Init(h)
	heap.Push(h, pqItem{moves: maxMoves, idNode: 0})
	visited := make(map[int]int)
	for h.Len() > 0 {
		item := heap.Pop(h).(pqItem)
		moves := item.moves
		idNode := item.idNode
		if _, ok := visited[idNode]; !ok {
			visited[idNode] = moves
			for neighbor := range graph[idNode] {
				newMoves := moves - graph[idNode][neighbor] - 1
				if _, ok := visited[neighbor]; !ok && newMoves >= 0 {
					heap.Push(h, pqItem{moves: newMoves, idNode: neighbor})
				}
			}
		}
	}
	ans := len(visited)
	for i := 0; i < len(edges); i++ {
		a := visited[edges[i][0]]
		b := visited[edges[i][1]]
		ans += min(a+b, edges[i][2])
	}
	return ans
}
