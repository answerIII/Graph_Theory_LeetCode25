package algo

import (
	//"fmt"

	"fmt"
	"log"
	"sort"

	"github.com/HikkMind/graph/structs"
	//"google.golang.org/appengine/log"
)

var (
	SavedWCC      *structs.Graph = nil
	SavedWCCCount int            = 0
)

func FindMaxWCC(graph structs.Graph, excludeVertex map[int]struct{}) (*structs.Graph, int) {
	if SavedWCC != nil {
		fmt.Println("using saved WCC")
		return SavedWCC, SavedWCCCount
	}
	graphWCC := structs.Graph{Directed: false}
	newAdjList := make(map[int][]int)
	for vertex, adj := range graph.AdjList {
		if _, ok := excludeVertex[vertex]; ok {
			continue
		}
		newSlice := make([]int, 0)
		for _, j := range adj {
			if _, ok := excludeVertex[j]; !ok {
				newSlice = append(newSlice, j)
			} else {
				graph.EdgesCount--
			}
		}
		// copy(newSlice, adj)
		newAdjList[vertex] = newSlice
	}
	graph.AdjList = newAdjList
	graph.VertexCount -= len(excludeVertex)

	if graph.Directed {
		for vertex, adj := range graph.AdjList {
			for _, j := range adj {
				graph.AdjList[j] = append(graph.AdjList[j], vertex)
			}
		}
	}
	// fmt.Println("WCC GRAPH : ", graph)

	visited := make(map[int]struct{})

	countWCC := 0

	for vertex := range graph.AdjList {
		tmpGraph := structs.Graph{AdjList: make(map[int][]int)}
		bfsQueue := make([]int, 0)
		if _, ok := visited[vertex]; ok {
			continue
		}
		// if _, ok := excludeVertex[vertex]; ok {
		// 	continue
		// }
		countWCC++

		bfsQueue = append(bfsQueue, vertex)
		tmpGraph.AdjList[vertex] = make([]int, 0)
		visited[vertex] = struct{}{}

		for len(bfsQueue) > 0 {
			actVertex := bfsQueue[0]
			actAdjVertex := graph.AdjList[actVertex]
			for _, i := range actAdjVertex {
				// if _, ok := excludeVertex[i]; ok {
				// 	continue
				// }
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

		if len(tmpGraph.AdjList) > graphWCC.VertexCount {
			graphWCC = tmpGraph
			graphWCC.VertexCount = len(tmpGraph.AdjList)
		}
	}
	graphWCC.EdgesCount /= 2

	if len(excludeVertex) == 0 {
		SavedWCC = &graphWCC
		SavedWCCCount = countWCC
	}

	return &graphWCC, countWCC
}

func FindMaxSCC(graph *structs.Graph) (int, int) {

	invGraph := InvertGraph(graph)
	dfsResult := TimeDFS(invGraph)
	// fmt.Println("INV GRAPH : ", invGraph)
	graph = InvertGraph(invGraph)
	// fmt.Println("SCC GRAPH : ", graph)

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
			// fmt.Println(sccCount, ":", dfsQueue[len(dfsQueue)-1])
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

func InvertGraph(graph *structs.Graph) *structs.Graph {
	if graph.Edges == nil {
		log.Fatal("cannot invert graph with no edges")
		return nil
	}

	outputGraph := structs.Graph{
		Directed:    graph.Directed,
		VertexCount: graph.VertexCount,
		EdgesCount:  len(graph.Edges),
		AdjList:     make(map[int][]int),
		Edges:       make([]structs.Edge, 0),
		// Edges:       make([]structs.Edge, len(graph.Edges)),
	}

	// copy(outputGraph.Edges, graph.Edges)

	for _, edge := range graph.Edges {
		v, u := edge.From, edge.To

		if adj, ok := outputGraph.AdjList[u]; ok {
			outputGraph.AdjList[u] = append(adj, v)
		} else {
			outputGraph.AdjList[u] = []int{v}
			// outputGraph.AdjList[u] = make([]int, 1)
			// outputGraph.AdjList[u][0] = v
		}
		outputGraph.Edges = append(outputGraph.Edges, structs.Edge{From: u, To: v})
	}

	return &outputGraph
}
