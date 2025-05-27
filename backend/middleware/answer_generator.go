package middleware

import (
	// "encoding/json"

	"encoding/json"
	"fmt"
	"sort"

	"github.com/HikkMind/graph/algo"
	"github.com/HikkMind/graph/structs"
)

func GenerateA1(graph *structs.Graph) []byte {

	answer := structs.AnswerA1{
		Directed:    graph.Directed,
		VertexCount: graph.VertexCount,
		EdgesCount:  graph.EdgesCount,
	}
	answer.Density = float64(graph.EdgesCount) / (float64(graph.VertexCount) * float64(graph.VertexCount-1) / 2)

	excludeVertex := make(map[int]struct{})
	// excludeVertex[7] = struct{}{}
	// excludeVertex[4] = struct{}{}
	answer.WCC, answer.WCCCount = algo.FindMaxWCC(*graph, excludeVertex)
	answer.ProportionWCC = float32(answer.WCC.VertexCount) / float32(graph.VertexCount-len(excludeVertex))
	// fmt.Println("WCC : ", answer.WCC)

	if graph.Directed {
		maxSCCSize, SCCCount := algo.FindMaxSCC(graph)
		answer.SCCCount = SCCCount
		answer.ProportionSCC = float32(maxSCCSize) / float32(graph.VertexCount)
	}

	output, _ := json.Marshal(answer)
	// output, _ := msgpack.Marshal(answer)

	return output
}

func GenerateA2(graph *structs.Graph, method string, verticesCount int) []byte {

	var graphDistances [][]int
	if method == "random_sample" {
		graphDistances = algo.RandomDistances(graph, verticesCount/2)
	} else if method == "snowball" {
		graphDistances = algo.RandomDistances(algo.SnowBall(graph, verticesCount), verticesCount/2)
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
	}
	output, _ := json.Marshal(answer)
	fmt.Println("answer2 : ", string(output))

	return output
}
