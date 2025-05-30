package benchmark

import (
	"fmt"
	"graph_theory/graph"
)

func getAccurateDistance(ugraph *graph.Graph, s, t int32) (int, error) {
	accurateDistance := -1

	stopCond := func(node graph.Node, distance int) bool {
		if node == graph.Node(t) {
			accurateDistance = distance
			return true
		}
		return false
	}

	graph.BFS(
		ugraph,
		[]graph.Node{graph.Node(s)},
		nil,
		nil,
		stopCond,
	)

	if accurateDistance == -1 {
		return -1, fmt.Errorf("can't find path between %d and %d\n", s, t)
	}

	return accurateDistance, nil
}
