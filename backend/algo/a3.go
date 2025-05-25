package algo

import (
	"fmt"
	"sync"

	"github.com/HikkMind/graph/structs"
)

func CountTriangles(graph *structs.Graph) structs.AnswerA3 {

	answer := structs.AnswerA3{}
	answer.AvgClusterCoef = AvgClusterCoef(graph)

	triangleCh := make(chan int)
	tripleCh := make(chan int) //max possible triples

	outputCh := make(chan []int)

	defer close(outputCh)

	var wg sync.WaitGroup
	for vertex := range graph.AdjList {
		wg.Add(1)
		go countTriples(graph, vertex, triangleCh, tripleCh, &wg)
	}

	go func(triangleCh, tripleCh <-chan int, outputCh chan<- []int) {
		totalTriangles := 0
		totalTriples := 0
		for {
			triangles, ok1 := <-triangleCh
			triples, ok2 := <-tripleCh
			if !ok1 && !ok2 {
				break
			}
			if ok1 {
				totalTriangles += triangles
			}
			if ok2 {
				totalTriples += triples
			}
		}

		outputCh <- []int{totalTriangles, totalTriples}
	}(triangleCh, tripleCh, outputCh)

	wg.Wait()
	close(triangleCh)
	close(tripleCh)
	totalCount := <-outputCh
	answer.TrianglesCount = totalCount[0] / 6
	answer.GlobalClusterCoef = float64(3*answer.TrianglesCount) / float64(totalCount[1])

	fmt.Println(totalCount)

	return answer
}

func countTriples(graph *structs.Graph, vertex int, triangleCh, tripleCh chan<- int, wg *sync.WaitGroup) {

	defer wg.Done()

	visited := make(map[int]struct{})
	for _, neighbor := range graph.AdjList[vertex] {
		visited[neighbor] = struct{}{}
	}

	trianglesCount := 0

	for _, i := range graph.AdjList[vertex] {
		for _, j := range graph.AdjList[i] {
			if _, ok := visited[j]; ok {
				trianglesCount++
			}
		}
	}

	triangleCh <- trianglesCount

	neighborCount := len(graph.AdjList[vertex])
	triplesCount := neighborCount * (neighborCount - 1) / 2

	tripleCh <- triplesCount

}
