package algo

import (
	//"fmt"

	"log"
	"sort"

	"github.com/HikkMind/graph/structs"
	//"google.golang.org/appengine/log"
)

func FindMaxWCC(graph *structs.Graph) structs.AnswerA1 {
	answer := structs.AnswerA1{WCC: structs.Graph{}}
	answer.Density = float64(graph.EdgesCount) / (float64(graph.VertexCount) * float64(graph.VertexCount-1) / 2)

	visited := make(map[int]struct{})

	for vertex := range graph.AdjList {
		tmpGraph := structs.Graph{AdjList: make(map[int][]int)}
		bfsQueue := make([]int, 0)
		if _, ok := visited[vertex]; ok {
			continue
		}
		answer.WCCCount++

		bfsQueue = append(bfsQueue, vertex)
		tmpGraph.AdjList[vertex] = make([]int, 0)
		visited[vertex] = struct{}{}

		for len(bfsQueue) > 0 {
			actVertex := bfsQueue[0]
			actAdjVertex := graph.AdjList[actVertex]
			for _, i := range actAdjVertex {
				tmpGraph.AdjList[actVertex] = append(tmpGraph.AdjList[actVertex], i)
				tmpGraph.EdgesCount++
				if _, ok := tmpGraph.AdjList[i]; !ok {
					tmpGraph.AdjList[i] = make([]int, 0)
				}
				if _, ok := visited[i]; !ok {
					bfsQueue = append(bfsQueue, i)
					visited[i] = struct{}{}
				}
			}
			bfsQueue = bfsQueue[1:]
		}

		if len(tmpGraph.AdjList) > answer.WCC.VertexCount {
			answer.WCC = tmpGraph
			answer.WCC.VertexCount = len(tmpGraph.AdjList)
		}
	}

	answer.VertexCount = answer.WCC.VertexCount
	answer.EdgesCount = answer.WCC.EdgesCount / 2
	answer.ProportionWCC = float32(answer.VertexCount) / float32(graph.VertexCount)

	if graph.Directed {
		maxScc, SCCCount := findMaxSCC(graph)
		answer.SCCCount = SCCCount
		answer.ProportionSCC = float32(maxScc) / float32(graph.VertexCount)
	}

	return answer
}

func findMaxSCC(graph *structs.Graph) (int, int) {

	invGraph := InvertGraph(graph)
	dfsResult := TimeDFS(&invGraph)

	dfsList := make([]structs.StatDFS, len(dfsResult))
	ind := 0
	for _, dfs := range dfsResult {
		dfsList[ind] = dfs
		ind++
	}

	sort.Slice(dfsList, func(i, j int) bool {
		return dfsList[i].TimeOut > dfsList[j].TimeOut
	})

	visited := make(map[int]struct{})

	sccCount := 0
	maxScc := 0

	dfsQueue := make([]int, 0)
	for _, startVertex := range dfsList {

		vertex := startVertex.Vertex

		if _, ok := visited[vertex]; ok {
			continue
		}

		sccCount++

		dfsQueue = append(dfsQueue, vertex)
		dfsQueueLen := 1
		powScc := 1
		visited[vertex] = struct{}{}

		for len(dfsQueue) > 0 {
			actVertex := dfsQueue[dfsQueueLen-1]
			isAdded := false
			for _, next := range graph.AdjList[actVertex] {
				if _, ok := visited[next]; !ok {
					dfsQueue = append(dfsQueue, next)
					dfsQueueLen++
					powScc++
					visited[next] = struct{}{}
					isAdded = true
					break
				}
			}
			if isAdded {
				continue
			}
			dfsQueue = dfsQueue[:dfsQueueLen-1]
			dfsQueueLen--
		}
		maxScc = max(maxScc, powScc)
	}

	return maxScc, sccCount

}

func TimeDFS(graph *structs.Graph) map[int]structs.StatDFS {
	visited := make(map[int]structs.StatDFS)
	dfsQueue := make([]int, 0)

	time := 1
	for vertex := range graph.AdjList {
		if _, ok := visited[vertex]; ok {
			continue
		}

		dfsQueue = append(dfsQueue, vertex)
		dfsQueueLen := 1
		visited[vertex] = structs.StatDFS{Vertex: vertex, TimeIn: time + 1}
		time++

		for len(dfsQueue) > 0 {
			actVertex := dfsQueue[dfsQueueLen-1]
			isAdded := false
			for _, next := range graph.AdjList[actVertex] {
				if _, ok := visited[next]; !ok {
					dfsQueue = append(dfsQueue, next)
					dfsQueueLen++
					visited[next] = structs.StatDFS{Vertex: next, TimeIn: time + 1}
					time++
					isAdded = true
					break
				}
			}
			if isAdded {
				continue
			}
			visited[actVertex] = structs.StatDFS{Vertex: actVertex, TimeIn: visited[actVertex].TimeIn, TimeOut: time + 1}
			time++
			dfsQueue = dfsQueue[:dfsQueueLen-1]
			dfsQueueLen--
		}
	}

	return visited

}

func InvertGraph(graph *structs.Graph) structs.Graph {
	if graph.Edges == nil {
		log.Fatal("cannot invert graph with no edges")
		return structs.Graph{}
	}

	outputGraph := structs.Graph{
		Directed:    graph.Directed,
		VertexCount: graph.VertexCount,
		EdgesCount:  graph.EdgesCount,
		AdjList:     make(map[int][]int),
		Edges:       make([]structs.Edge, len(graph.Edges)),
	}

	copy(outputGraph.Edges, graph.Edges)

	for _, edge := range outputGraph.Edges {
		v, u := edge.From, edge.To

		if adj, ok := outputGraph.AdjList[u]; ok {
			outputGraph.AdjList[u] = append(adj, v)
		} else {
			outputGraph.AdjList[u] = make([]int, 1)
			outputGraph.AdjList[u][0] = v
		}
	}

	return outputGraph
}
