package middleware

import (
	// "encoding/json"

	"encoding/json"
	"fmt"
	"os"
	"sort"
	"strconv"
	"sync"
	"time"

	"math/rand"

	"github.com/HikkMind/graph/algo"
	"github.com/HikkMind/graph/structs"
)

func GenerateProperties(graph *structs.Graph) []byte {

	var startTime time.Time
	answer := structs.AnswerA1{
		Directed:    graph.Directed,
		VertexCount: graph.VertexCount,
		EdgesCount:  graph.EdgesCount,
	}
	answer.Density = float64(graph.EdgesCount) / (float64(graph.VertexCount) * float64(graph.VertexCount-1) / 2)

	excludeVertex := make(map[int]struct{})
	// excludeVertex[7] = struct{}{}
	// excludeVertex[4] = struct{}{}
	startTime = time.Now()
	answer.WCC, answer.WCCCount = algo.FindMaxWCC(*graph, excludeVertex)
	answer.TimeWCCms = int(time.Since(startTime).Milliseconds())
	answer.ProportionWCC = float32(answer.WCC.VertexCount) / float32(graph.VertexCount-len(excludeVertex))
	// fmt.Println("WCC : ", answer.WCC)

	if graph.Directed {
		startTime = time.Now()
		maxSCCSize, SCCCount := algo.FindMaxSCC(graph)
		answer.TimeSCCms = int(time.Since(startTime).Milliseconds())
		answer.SCCCount = SCCCount
		answer.ProportionSCC = float32(maxSCCSize) / float32(graph.VertexCount)
	}

	output, _ := json.Marshal(answer)
	// output, _ := msgpack.Marshal(answer)

	return output
}

func GenerateDistances(graph *structs.Graph, method string, verticesCount int) []byte {

	var algoTime int

	var graphDistances [][]int
	if method == "random_sample" {
		startTime := time.Now()
		graphDistances = algo.RandomDistances(graph, verticesCount/2)
		algoTime = int(time.Since(startTime).Milliseconds())
	} else if method == "snowball" {
		startTime := time.Now()
		graphDistances = algo.RandomDistances(algo.SnowBall(graph, verticesCount), verticesCount/2)
		algoTime = int(time.Since(startTime).Milliseconds())
	} else {
		panic("distance : unknown method")
	}

	sort.Slice(graphDistances, func(i, j int) bool {
		return graphDistances[i][0] < graphDistances[j][0]
	})

	var meanDistance float32 = 0
	var distanceCount float32 = float32(len(graphDistances))

	for _, d := range graphDistances {
		meanDistance += float32(d[0]) / distanceCount
	}

	answer := structs.AnswerA2{
		Percentile90: graphDistances[int(0.9*distanceCount)][0],
		Diameter:     algo.FindDiameter(graph),
		MeanDistance: meanDistance,
		TimeMs:       algoTime,
	}
	output, _ := json.Marshal(answer)

	return output
}

func GenerateClustering(graph *structs.Graph) []byte {

	var answer structs.AnswerA3 = algo.CountTriangles(graph)
	startTime := time.Now()
	graphWCC, _ := algo.FindMaxWCC(*graph, make(map[int]struct{}))
	answer.AvgClusterCoefWCC.Value = algo.AvgClusterCoef(graphWCC)
	answer.AvgClusterCoefWCC.TimeMs = int(time.Since(startTime).Milliseconds())

	output, _ := json.Marshal(answer)

	return output

}

func GenerateDegrees(graph *structs.Graph) []byte {

	startTime := time.Now()
	answer := algo.GetDegreeProbability(graph)
	answer.TimeMs = int(time.Since(startTime).Milliseconds())

	output, _ := json.Marshal(answer)
	return output

}

func GenerateRobustness(graph *structs.Graph, method string, percent int) []byte {

	var excludeVertex map[int]struct{}
	startTime := time.Now()
	// if method == "random" {
	// 	excludeVertex = algo.GetRandomVertexSet(graph, float32(percent))
	// } else if method == "targeted" {
	// 	excludeVertex = algo.GetMaxDegreeVertexSet(graph, float32(percent))
	// }

	percentPropWCC := make(map[int]float32)
	goroutineCh := make(chan int)
	var wg sync.WaitGroup
	goroutineCount, err := strconv.Atoi(os.Getenv("GOROUTINECOUNT"))
	if err != nil || goroutineCount < 0 {
		goroutineCount = len(graph.AdjList)
		fmt.Println("use max goroutine count : ", goroutineCount)
	}
	for range goroutineCount {
		wg.Add(1)
		go func(graph *structs.Graph, percentPropWCC map[int]float32, goroutineCh <-chan int, wg *sync.WaitGroup) {
			defer wg.Done()
			for {
				percent, ok := <-goroutineCh
				if !ok {
					break
				}
				if method == "random" {
					excludeVertex = algo.GetRandomVertexSet(graph, float32(percent))
				} else if method == "targeted" {
					excludeVertex = algo.GetMaxDegreeVertexSet(graph, float32(percent))
				}
				graphWCC, _ := algo.FindMaxWCC(*graph, excludeVertex)
				percentPropWCC[percent] = float32(graphWCC.VertexCount) / (float32(graph.VertexCount - len(excludeVertex)))

			}
		}(graph, percentPropWCC, goroutineCh, &wg)
	}

	go func(goroutineCh chan<- int) {
		for percent := 0; percent < 100; percent++ {
			goroutineCh <- percent
			fmt.Println(percent)
		}
		close(goroutineCh)
	}(goroutineCh)

	// for percent := 0; percent < 100; percent++ {
	// 	graphWCC, _ := algo.FindMaxWCC(*graph, excludeVertex)
	// 	percentPropWCC[percent] = float32(graphWCC.VertexCount) / (float32(graph.VertexCount - len(excludeVertex)))
	// }

	wg.Wait()

	answer := structs.AnswerB{
		Percentage: percent,
		Method:     method,
		// ProportionWCC: float32(graphWCC.VertexCount) / (float32(graph.VertexCount - len(excludeVertex))),
		ProportionWCC: percentPropWCC,
		TimeMs:        int(time.Since(startTime).Milliseconds()),
	}

	output, _ := json.Marshal(answer)

	return output

}

func GenerateRandomNodes(graph *structs.Graph) []byte {
	type Nodes struct {
		Node1 int `json:"node1"`
		Node2 int `json:"node2"`
	}

	graphWCC, _ := algo.FindMaxWCC(*graph, make(map[int]struct{}))

	vertexList := make([]int, graphWCC.VertexCount)
	ind := 0
	for vertex := range graphWCC.VertexCount {
		vertexList[ind] = vertex
		ind++
	}
	randLocal := rand.New(rand.NewSource(time.Now().UnixNano()))
	randLocal.Shuffle(len(vertexList), func(i, j int) {
		vertexList[i], vertexList[j] = vertexList[j], vertexList[i]
	})

	// fmt.Println(vertexList)
	// fmt.Println(graphWCC.VertexCount, len(graphWCC.AdjList))

	var answer Nodes
	if len(vertexList) < 2 {
		answer = Nodes{
			Node1: -1,
			Node2: -1,
		}
	} else {
		answer = Nodes{
			Node1: vertexList[0],
			Node2: vertexList[1],
		}
	}

	output, _ := json.Marshal(answer)

	return output

}
