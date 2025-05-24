package algo

import (
	"sync"

	"github.com/HikkMind/graph/structs"
)

func AvgClusterCoef(graph *structs.Graph) float64 {

	var answer float64 = 0
	vertexCount := float64(graph.VertexCount)
	inputCh := make(chan float64)
	outputCh := make(chan float64)
	defer close(outputCh)

	var wg sync.WaitGroup

	for vertex := range graph.AdjList {
		wg.Add(1)
		go clusterCoef(graph, vertex, inputCh, &wg)
	}

	go func(inputCh <-chan float64, outputCh chan<- float64, vertexCount float64) {
		var answer float64 = 0
		for {
			value, ok := <-inputCh
			if !ok {
				outputCh <- answer
				break
			}
			answer += value / vertexCount

		}
	}(inputCh, outputCh, vertexCount)

	// for vertex := range graph.AdjList {
	// 	// fmt.Println("cluster ", vertex, " : ", clusterCoef(graph, vertex)/vertexCount)
	// 	answer += clusterCoef(graph, vertex) / vertexCount
	// }
	wg.Wait()
	close(inputCh)
	answer = <-outputCh

	return answer
}

func clusterCoef(graph *structs.Graph, vertex int, inputCh chan<- float64, wg *sync.WaitGroup) {

	defer wg.Done()
	neighborsCount := float64(len(graph.AdjList[vertex]))
	if neighborsCount < 2 {
		// return 0
		return
	}

	neighborsEdgesBetween := 0
	visited := make(map[int]struct{})

	for _, neighbor := range graph.AdjList[vertex] {
		visited[neighbor] = struct{}{}
	}

	for _, i := range graph.AdjList[vertex] {
		for _, j := range graph.AdjList[i] {
			if _, ok := visited[j]; ok {
				neighborsEdgesBetween++
			}
		}
	}
	answer := float64(neighborsEdgesBetween) / (neighborsCount - 1) / neighborsCount
	inputCh <- answer
}
