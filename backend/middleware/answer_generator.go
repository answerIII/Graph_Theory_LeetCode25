package middleware

import (
	// "encoding/json"

	"encoding/json"
	"sort"

	"github.com/HikkMind/graph/algo"
	"github.com/HikkMind/graph/structs"
)

func GenerateProperties(graph *structs.Graph) []byte {

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

func GenerateDistances(graph *structs.Graph, method string, verticesCount int) []byte {

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

	return output
}

func GenerateClustering(graph *structs.Graph) []byte {

	var answer structs.AnswerA3 = algo.CountTriangles(graph)
	graphWCC, _ := algo.FindMaxWCC(*graph, make(map[int]struct{}))
	answer.AvgClusterCoefWCC = algo.AvgClusterCoef(graphWCC)

	output, _ := json.Marshal(answer)

	return output

}

func GenerateDegrees(graph *structs.Graph) []byte {

	answer := algo.GetDegreeProbability(graph)

	output, _ := json.Marshal(answer)
	return output

}

func GenerateRobustness(graph *structs.Graph, method string, percent int) []byte {

	var excludeVertex map[int]struct{}
	if method == "random" {
		excludeVertex = algo.GetRandomVertexSet(graph, float32(percent))
	} else if method == "targeted" {
		excludeVertex = algo.GetMaxDegreeVertexSet(graph, float32(percent))
	}

	graphWCC, _ := algo.FindMaxWCC(*graph, excludeVertex)

	answer := structs.AnswerB{Percentage: percent, Method: method, ProportionWCC: float32(graphWCC.VertexCount) / (float32(graph.VertexCount - len(excludeVertex)))}

	output, _ := json.Marshal(answer)

	return output

}
