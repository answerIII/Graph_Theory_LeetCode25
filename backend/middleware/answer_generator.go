package middleware

import (
	"encoding/json"

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

	answer.WCC, answer.WCCCount = algo.FindMaxWCC(*graph)
	answer.ProportionWCC = float32(answer.WCC.VertexCount) / float32(graph.VertexCount)

	if graph.Directed {
		maxSCCSize, SCCCount := algo.FindMaxSCC(graph)
		answer.SCCCount = SCCCount
		answer.ProportionSCC = float32(maxSCCSize) / float32(graph.VertexCount)
	}

	output, _ := json.Marshal(answer)

	return output
}
