package graph

import "math/rand/v2"

func generateSampleNodes(
	nodes []Node,
	sampleN int,
) [][]Node {
	sampleN = int(min(int64(sampleN), int64(len(nodes))*int64(len(nodes)-1)/2))
	type pair struct {
		u, v Node
	}
	sampleNodes := make(map[pair]struct{})
	compLen := len(nodes)

	for len(sampleNodes) < sampleN {
		u := nodes[rand.IntN(compLen)]
		v := nodes[rand.IntN(compLen)]
		if u == v {
			continue
		}
		if u > v {
			u, v = v, u
		}
		p := pair{u, v}
		if _, exists := sampleNodes[p]; !exists {
			sampleNodes[p] = struct{}{}
		}
	}

	res := make([][]Node, 0, len(sampleNodes))
	for k := range sampleNodes {
		res = append(res, []Node{k.u, k.v})
	}

	return res
}

func generateSnowballSample(
	graph *Graph,
	component []Node,
) []Node {
	startNode := component[rand.IntN(len(component))]
	neighbors := graph.Adj.neighbors(startNode)

	nodes := make([]Node, 0, 3)
	nodes = append(nodes, startNode)
	nodes = append(nodes, neighbors[rand.IntN(len(neighbors))])
	if len(neighbors) > 1 {
		newNode := neighbors[rand.IntN(len(neighbors))]
		for newNode == nodes[1] {
			newNode = neighbors[rand.IntN(len(neighbors))]
		}
		nodes = append(nodes, newNode)
	}
	return nodes
}
