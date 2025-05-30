package algo

import (
	"fmt"
	"os"
	"strconv"
	"sync"
	"time"

	"github.com/HikkMind/graph/structs"
)

func CountTriangles(graph *structs.Graph) structs.AnswerA3 {

	answer := structs.AnswerA3{}
	startTime := time.Now()
	answer.AvgClusterCoef.Value = AvgClusterCoef(graph)
	answer.AvgClusterCoef.TimeMs = int(time.Since(startTime).Milliseconds())

	startTime = time.Now()

	triangleCh := make(chan int)
	tripleCh := make(chan int) //max possible triples

	goroutineCh := make(chan int)
	outputCh := make(chan []int)

	defer close(outputCh)

	var wg sync.WaitGroup
	goroutineCount, err := strconv.Atoi(os.Getenv("GOROUTINECOUNT"))
	if err != nil || goroutineCount < 0 {
		goroutineCount = len(graph.AdjList)
		fmt.Println("use max goroutine count : ", goroutineCount)
	}
	// fmt.Println("goroutine count : ", goroutineCount)
	for range goroutineCount {
		wg.Add(1)
		go func(graph *structs.Graph, triangleCh, tripleCh chan<- int, wg *sync.WaitGroup) {
			defer wg.Done()
			for {
				vertex, ok := <-goroutineCh
				if !ok {
					break
				}
				countTriples(graph, vertex, triangleCh, tripleCh)
			}
		}(graph, triangleCh, tripleCh, &wg)
	}

	go func(graph *structs.Graph) {
		for vertex := range graph.AdjList {
			goroutineCh <- vertex
		}
		close(goroutineCh)
	}(graph)

	// for vertex := range graph.AdjList {
	// 	wg.Add(1)
	// 	go countTriples(graph, vertex, triangleCh, tripleCh, &wg)
	// }

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
	answer.TrianglesCount.TimeMs = int(time.Since(startTime).Milliseconds())
	answer.TrianglesCount.Value = float64(totalCount[0] / 6)
	answer.GlobalClusterCoef.Value = float64(3*answer.TrianglesCount.Value) / float64(totalCount[1])

	return answer
}

func countTriples(graph *structs.Graph, vertex int, triangleCh, tripleCh chan<- int) {

	// defer wg.Done()

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
