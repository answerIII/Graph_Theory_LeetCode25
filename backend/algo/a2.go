package algo

import (
	"math/rand"
	"sync"
	"time"

	"github.com/HikkMind/graph/structs"
)

func FindDiameter(graph structs.Graph) int {

	var startVertex, vertex, vertexDist int
	for v := range graph.AdjList {
		startVertex = v
		break
	}

	visited := make(map[int]struct{})
	visited[startVertex] = struct{}{}
	bfsQueue := make([]int, 0)
	bfsQueue = append(bfsQueue, startVertex)

	for len(bfsQueue) > 0 {
		vertex = bfsQueue[0]
		bfsQueue = bfsQueue[1:]
		for next := range graph.AdjList[vertex] {
			if _, ok := visited[next]; !ok {
				visited[next] = struct{}{}
				bfsQueue = append(bfsQueue, next)
			}
		}
	}

	startVertex = vertex
	for k := range visited {
		delete(visited, k)
	}
	visited[startVertex] = struct{}{}

	bfsQueueDist := make([][]int, 0)
	bfsQueueDist = append(bfsQueueDist, []int{startVertex, 0})

	for len(bfsQueueDist) > 0 {
		vertex = bfsQueueDist[0][0]
		vertexDist = bfsQueueDist[0][1]
		for _, next := range graph.AdjList[vertex] {
			if _, ok := visited[next]; !ok {
				visited[next] = struct{}{}
				bfsQueueDist = append(bfsQueueDist, []int{next, vertexDist + 1})
			}
		}
		bfsQueueDist = bfsQueueDist[1:]
	}

	return vertexDist
}

func FindDistance(graph *structs.Graph, source, target int) int {

	if source == target {
		return 0
	}

	var vertex, vertexDist int
	visited := make(map[int]struct{})
	bfsQueueDist := make([][]int, 0)
	bfsQueueDist = append(bfsQueueDist, []int{source, 0})

	for len(bfsQueueDist) > 0 {
		//fmt.Println("QUEUE : ", bfsQueueDist)
		vertex = bfsQueueDist[0][0]
		vertexDist = bfsQueueDist[0][1]
		for _, next := range graph.AdjList[vertex] {
			if next == target {
				vertexDist++
				break
			}
			if _, ok := visited[next]; !ok {
				visited[next] = struct{}{}
				bfsQueueDist = append(bfsQueueDist, []int{next, vertexDist + 1})
			}
		}
		bfsQueueDist = bfsQueueDist[1:]
	}

	return vertexDist
}

func RandomDistances(graph *structs.Graph, pairCount int) [][]int {
	result := make([][]int, pairCount)
	randLocal := rand.New(rand.NewSource(time.Now().UnixNano()))

	var wg sync.WaitGroup
	wg.Add(pairCount)
	vertexCount := len(graph.AdjList)
	vertexList := make([]int, vertexCount)
	ind := 0
	for vertex := range graph.AdjList {
		vertexList[ind] = vertex
		ind++
	}
	randLocal.Shuffle(len(vertexList), func(i, j int) {
		vertexList[i], vertexList[j] = vertexList[j], vertexList[i]
	})

	var source, target int
	for i := 0; i < pairCount && i*2 < vertexCount; i++ {
		k := 2
		for vertex := range graph.AdjList {
			source = target
			target = vertex
			k--
			if k == 0 {
				break
			}
		}
		result[i] = []int{FindDistance(graph, source, target), source, target}
		wg.Done()
		// go func(i int) {
		// }(i)
	}

	wg.Wait()

	return result

}
