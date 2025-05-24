package algo

import (
	"github.com/HikkMind/graph/structs"
)

func GetDegreeProbability(graph *structs.Graph) structs.AnswerA5 {

	vertexCount := graph.VertexCount

	answer := structs.AnswerA5{MaxDegree: 0, AvgDegree: float64(graph.EdgesCount) * 2 / float64(vertexCount), MinDegree: vertexCount, ProbabilityDegree: make(map[int]float64)}

	for _, adj := range graph.AdjList {
		answer.ProbabilityDegree[len(adj)] += 1
		answer.MaxDegree = max(answer.MaxDegree, len(adj))
		answer.MinDegree = min(answer.MinDegree, len(adj))
	}

	for degree := range answer.ProbabilityDegree {
		answer.ProbabilityDegree[degree] /= float64(vertexCount)
	}

	return answer
}
