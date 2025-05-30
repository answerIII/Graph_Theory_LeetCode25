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

	var graphDistances [][]int = make([][]int, 0)

	passCount, _ := strconv.Atoi(os.Getenv("PASSNUMBER"))

	graphWCC, _ := algo.FindMaxWCC(*graph, make(map[int]struct{}))
	for range passCount {
		if method == "random_sample" {
			startTime := time.Now()
			graphDistances = append(graphDistances, algo.RandomDistances(graphWCC, verticesCount/2)...)
			algoTime = int(time.Since(startTime).Milliseconds())
		} else if method == "snowball" {
			startTime := time.Now()
			graphDistances = append(graphDistances, algo.RandomDistances(algo.SnowBall(graphWCC, verticesCount), verticesCount/2)...)
			algoTime = int(time.Since(startTime).Milliseconds())
		} else if method == "random_sample_snowball" {
			startTime := time.Now()
			graphDistances = append(graphDistances, algo.RandomDistances(graphWCC, verticesCount/2)...)
			graphDistances = append(graphDistances, algo.RandomDistances(algo.SnowBall(graphWCC, verticesCount), verticesCount/2)...)
			algoTime = int(time.Since(startTime).Milliseconds())
		} else {
			panic("distance : unknown method")
		}
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
		Method:       method,
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

func GenerateRobustness(graph *structs.Graph, percentage []int) []byte {

	// var excludeVertex map[int]struct{}
	// if method == "random" {
	// 	excludeVertex = algo.GetRandomVertexSet(graph, float32(percent))
	// } else if method == "targeted" {
	// 	excludeVertex = algo.GetMaxDegreeVertexSet(graph, float32(percent))
	// }

	// targetPropWCC := make(map[int]float32)
	// targetPropWCC[0] = 1
	goroutineCh := make(chan int)
	answerCh := make(chan structs.AnswerB)
	passCount, _ := strconv.Atoi(os.Getenv("PASSNUMBER"))

	var wg sync.WaitGroup
	goroutineCount, err := strconv.Atoi(os.Getenv("GOROUTINECOUNT"))
	if err != nil || goroutineCount < 0 {
		goroutineCount = len(graph.AdjList)
		fmt.Println("use max goroutine count : ", goroutineCount)
	}
	for range goroutineCount {
		wg.Add(1)
		go func(graph *structs.Graph, goroutineCh <-chan int, answerCh chan<- structs.AnswerB, wg *sync.WaitGroup) {
			defer wg.Done()
			for {
				percent, ok := <-goroutineCh
				if !ok {
					break
				}
				var excludeVertex map[int]struct{}
				timeStart := time.Now()
				ans := structs.AnswerB{Percentage: percent}
				ans.RandomFraction = 0
				var graphWCC *structs.Graph
				for range passCount {
					excludeVertex = algo.GetRandomVertexSet(graph, float32(percent))
					graphWCC, _ = algo.FindMaxWCC(*graph, excludeVertex)
					ans.RandomFraction += float32(graphWCC.VertexCount) / (float32(graph.VertexCount - len(excludeVertex)))
				}
				ans.RandomFraction /= float32(passCount)

				excludeVertex = algo.GetMaxDegreeVertexSet(graph, float32(percent))
				graphWCC, _ = algo.FindMaxWCC(*graph, excludeVertex)
				ans.TargetFraction = float32(graphWCC.VertexCount) / (float32(graph.VertexCount - len(excludeVertex)))

				ans.TimeMs = int(time.Since(timeStart).Milliseconds())
				// targetPropWCC[percent] = float32(graphWCC.VertexCount) / (float32(graph.VertexCount - len(excludeVertex)))
				answerCh <- ans
				// fmt.Println(percent, graphWCC.VertexCount, len(excludeVertex))

			}
		}(graph, goroutineCh, answerCh, &wg)
	}

	go func(percentage []int, goroutineCh chan<- int) {
		for _, percent := range percentage {
			goroutineCh <- percent
		}
		close(goroutineCh)
	}(percentage, goroutineCh)

	// answer := make([]structs.AnswerB, len(percentage))
	answer := make([]structs.AnswerB, len(percentage))
	signalCh := make(chan struct{})
	go func(answerCh <-chan structs.AnswerB, signalCh chan struct{}, answer *[]structs.AnswerB) {
		ind := 0
		for {
			t, ok := <-answerCh
			if !ok {
				break
			}
			if t.Percentage == 100 {
				t.RandomFraction = 0
				t.TargetFraction = 0
			}
			(*answer)[ind] = t
			ind++
		}
		signalCh <- struct{}{}
	}(answerCh, signalCh, &answer)

	// for percent := 0; percent < 100; percent++ {
	// 	graphWCC, _ := algo.FindMaxWCC(*graph, excludeVertex)
	// 	percentPropWCC[percent] = float32(graphWCC.VertexCount) / (float32(graph.VertexCount - len(excludeVertex)))
	// }

	wg.Wait()
	close(answerCh)
	<-signalCh

	// answer := structs.AnswerB{
	// 	Percentage: percent,
	// 	Method:     method,
	// 	// ProportionWCC: float32(graphWCC.VertexCount) / (float32(graph.VertexCount - len(excludeVertex))),
	// 	ProportionWCC: targetPropWCC,
	// 	TimeMs:        int(time.Since(startTime).Milliseconds()),
	// }
	sort.Slice(answer, func(i, j int) bool {
		return answer[i].Percentage < answer[j].Percentage
	})
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

func GenerateClusteringVertex(graph *structs.Graph, vertex int) []byte {
	type ClusterVertexCoef struct {
		Vertex      int     `json:"vertex"`
		ClusterCoef float64 `json:"cluster_coef"`
	}

	var answer ClusterVertexCoef

	ch := make(chan float64)
	answer.Vertex = vertex
	go algo.ClusterCoef(graph, vertex, ch)
	answer.ClusterCoef = <-ch

	output, _ := json.Marshal(answer)

	return output

}
