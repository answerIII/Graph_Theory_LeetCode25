package algo

import (
	"log"
	"math/rand"
	"sync"
	"time"

	//"github.com/HikkMind/graph/algo"
	"github.com/HikkMind/graph/structs"
)

func FindDiameter(graph *structs.Graph) int {

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
		for _, next := range graph.AdjList[vertex] {
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

func FindDistanceBFS(graph *structs.Graph, source, target int) int {

	if source == target {
		return 0
	}

	var vertex, vertexDist int
	visited := make(map[int]struct{})
	visited[source] = struct{}{}
	bfsQueueDist := make([][]int, 0)
	bfsQueueDist = append(bfsQueueDist, []int{source, 0})

	for len(bfsQueueDist) > 0 {
		//fmt.Println("QUEUE : ", bfsQueueDist)
		vertex = bfsQueueDist[0][0]
		vertexDist = bfsQueueDist[0][1]
		for _, next := range graph.AdjList[vertex] {
			if next == target {
				return vertexDist + 1
			}
			if _, ok := visited[next]; !ok {
				visited[next] = struct{}{}
				bfsQueueDist = append(bfsQueueDist, []int{next, vertexDist + 1})
			}
		}
		bfsQueueDist = bfsQueueDist[1:]
	}

	return -1
}

func FindDistanceDFS(graph *structs.Graph, source, target, diameter int) int {
	if source == target {
		return 0
	}

	var vertex, vertexDist, dfsQueueLen int
	var isAdded bool
	visited := make(map[int]struct{})
	dfsQueueDist := make([][]int, 0)
	dfsQueueDist = append(dfsQueueDist, []int{source, 0, 0})
	dfsQueueLen = 1

	answer := diameter + 1

	for dfsQueueLen > 0 {

		isAdded = false
		vertex = dfsQueueDist[dfsQueueLen-1][0]
		vertexDist = dfsQueueDist[dfsQueueLen-1][1]
		edgeInd := dfsQueueDist[dfsQueueLen-1][2]
		visited[vertex] = struct{}{}
		for i := edgeInd; i < len(graph.AdjList[vertex]); i++ {
			next := graph.AdjList[vertex][i]
			if next == target {
				answer = min(answer, vertexDist+1)
				break
			}
			if _, ok := visited[next]; !ok && vertexDist < diameter {
				dfsQueueDist[dfsQueueLen-1][2] = i + 1
				dfsQueueDist = append(dfsQueueDist, []int{next, vertexDist + 1, 0})
				dfsQueueLen++
				isAdded = true
				break
			}
		}
		if !isAdded {
			delete(visited, vertex)
			dfsQueueDist = dfsQueueDist[:dfsQueueLen-1]
			dfsQueueLen--
		}
	}

	if answer <= diameter {
		return answer
	}
	return -1
}

func RandomDistances(graph *structs.Graph, pairCount int) [][]int {

	if pairCount/2 > graph.VertexCount {
		log.Fatal("randomdistance : too many pair count")
		return nil
	}

	result := make([][]int, pairCount)
	randLocal := rand.New(rand.NewSource(time.Now().UnixNano()))

	var wg sync.WaitGroup
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

	for i := 0; i+1 < vertexCount && i/2 < pairCount; i += 2 {
		source = vertexList[i]
		target = vertexList[i+1]
		result[i/2] = []int{FindDistanceBFS(graph, source, target), source, target}
	}

	wg.Wait()

	return result

}

func SnowBall(graph *structs.Graph, vertexCount int) *structs.Graph {

	outputGraph := structs.Graph{
		Directed:    false,
		VertexCount: 0,
		EdgesCount:  0,
		AdjList:     make(map[int][]int),
	}

	var startVertex int

	for vertex, adj := range graph.AdjList {
		if len(adj) > 0 {
			startVertex = vertex
			break
		}
	}

	bfsQueue := []int{startVertex, graph.AdjList[startVertex][0]}

	outputGraph.AdjList[bfsQueue[0]] = []int{}
	outputGraph.AdjList[bfsQueue[1]] = []int{}
	outputGraph.VertexCount = 2

	for outputGraph.VertexCount < vertexCount && len(bfsQueue) > 0 {
		vertex := bfsQueue[0]
		bfsQueue = bfsQueue[1:]
		for _, next := range graph.AdjList[vertex] {
			if outputGraph.VertexCount == vertexCount {
				break
			}
			if _, ok := outputGraph.AdjList[next]; !ok {
				outputGraph.AdjList[next] = []int{}
				bfsQueue = append(bfsQueue, next)
				outputGraph.VertexCount++
			}
		}
	}

	for vertex := range outputGraph.AdjList {
		for _, next := range graph.AdjList[vertex] {
			if _, ok := outputGraph.AdjList[next]; ok {
				outputGraph.AdjList[vertex] = append(outputGraph.AdjList[vertex], next)
				outputGraph.EdgesCount++
			}
		}
	}

	outputGraph.EdgesCount /= 2

	return &outputGraph
}
