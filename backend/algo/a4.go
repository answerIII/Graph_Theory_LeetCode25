package algo

import (
	"fmt"
	"os"
	"strconv"
	"sync"

	"github.com/HikkMind/graph/structs"
)

func AvgClusterCoef(graph *structs.Graph) float64 {

	var answer float64 = 0
	vertexCount := float64(graph.VertexCount)
	inputCh := make(chan float64)
	outputCh := make(chan float64)
	goroutineCh := make(chan int)
	defer close(outputCh)

	var wg sync.WaitGroup
	goroutineCount, err := strconv.Atoi(os.Getenv("GOROUTINECOUNT"))
	if err != nil || goroutineCount < 0 {
		goroutineCount = len(graph.AdjList)
		fmt.Println("use max goroutine count : ", goroutineCount)
	}
	for range goroutineCount {
		wg.Add(1)
		go func(graph *structs.Graph, inputCh chan<- float64, wg *sync.WaitGroup) {
			defer wg.Done()
			for {
				vertex, ok := <-goroutineCh
				if !ok {
					break
				}
				ClusterCoef(graph, vertex, inputCh)
			}
		}(graph, inputCh, &wg)
	}

	go func(graph *structs.Graph) {
		for vertex := range graph.AdjList {
			goroutineCh <- vertex
		}
		close(goroutineCh)
	}(graph)

	// for vertex := range graph.AdjList {
	// 	wg.Add(1)
	// 	go clusterCoef(graph, vertex, inputCh, &wg)
	// }

	go func(inputCh <-chan float64, outputCh chan<- float64, vertexCount float64) {
		var answer float64 = 0
		for {
			value, ok := <-inputCh
			if !ok {
				break
			}
			answer += value / vertexCount
		}
		outputCh <- answer
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

func ClusterCoef(graph *structs.Graph, vertex int, inputCh chan<- float64) {
	// func clusterCoef(graph *structs.Graph, vertex int, inputCh chan<- float64, wg *sync.WaitGroup) {

	// defer wg.Done()
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
