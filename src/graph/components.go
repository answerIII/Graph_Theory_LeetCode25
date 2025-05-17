package graph

import (
	"errors"
	"sort"
)

func (g *Graph) FindWCC() ([][]Node, error) {
	if g.Directed {
		return nil, errors.New("can't find WCC in directed graph")
	}
	return DFS(g, g.getNodesSlice(), nil, nil, nil), nil
}

func (g *Graph) FindSCC() ([][]Node, error) {
	if !g.Directed {
		return nil, errors.New("can't find SCC in undirected graph")
	}

	nodes := g.getNodesSlice()

	tout := make(map[Node]int)
	time := 0

	onOut := func(node Node) {
		tout[node] = time
		time++
	}

	DFS(g, nodes, nil, nil, onOut)

	sort.Slice(nodes, func(i, j int) bool {
		return tout[nodes[i]] > tout[nodes[j]]
	})

	inverted := g.Inverted()

	return DFS(inverted, nodes, nil, nil, nil), nil
}

func SortComponents(components [][]Node, desc bool) [][]Node {
	sort.Slice(components, func(i, j int) bool {
		if desc {
			return len(components[i]) > len(components[j])
		}
		return len(components[i]) < len(components[j])
	})
	return components
}
