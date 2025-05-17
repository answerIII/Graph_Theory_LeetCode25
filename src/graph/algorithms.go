package graph

import (
	"sort"
)

func DFS(
	graph *Graph,
	startNodes []Node,
	used map[Node]struct{},
	onIn, onOut func(Node),
) [][]Node {
	if used == nil {
		used = make(map[Node]struct{})
	}

	var components [][]Node

	for _, node := range startNodes {
		if _, ok := used[node]; ok {
			continue
		}

		var stack []struct {
			node        Node
			isProcessed bool
		}
		var path []Node

		stack = append(stack, struct {
			node        Node
			isProcessed bool
		}{node, false})
		used[node] = struct{}{}

		for len(stack) > 0 {
			lastIdx := len(stack) - 1
			item := stack[lastIdx]
			stack = stack[:lastIdx]

			u := item.node
			if item.isProcessed {
				if onOut != nil {
					onOut(u)
				}
				path = append(path, u)
			} else {
				if onIn != nil {
					onIn(u)
				}

				stack = append(stack, struct {
					node        Node
					isProcessed bool
				}{u, true})

				neighbors := graph.GetNeighborsRandomSlice(u)
				sort.Slice(neighbors, func(i, j int) bool {
					return neighbors[i] < neighbors[j]
				})

				for _, v := range neighbors {
					if _, ok := used[v]; !ok {
						stack = append(stack, struct {
							node        Node
							isProcessed bool
						}{v, false})
						used[v] = struct{}{}
					}
				}
			}
		}

		components = append(components, path)
	}

	return components
}

func BFS(
	graph *Graph,
	startNodes []Node,
	used map[Node]struct{},
	onVisit func(Node, Node, int),
	stopCond func(Node, int) bool,
) (map[Node]int, error) {
	if used == nil {
		used = make(map[Node]struct{})
	}

	distances := make(map[Node]int)

	for _, node := range startNodes {
		distances[node] = 0
		queue := []Node{node}
		used[node] = struct{}{}

		for len(queue) > 0 {
			u := queue[0]
			queue = queue[1:]

			neighbors := graph.GetNeighborsRandomSlice(u)
			sort.Slice(neighbors, func(i, j int) bool {
				return neighbors[i] < neighbors[j]
			})

			for _, neighbor := range neighbors {
				if _, ok := used[neighbor]; !ok {
					used[neighbor] = struct{}{}
					queue = append(queue, neighbor)
					distances[neighbor] = distances[u] + 1
					if onVisit != nil {
						onVisit(neighbor, u, distances[neighbor])
					}
					if stopCond != nil && stopCond(neighbor, distances[neighbor]) {
						return distances, nil
					}
				}
			}
		}
	}

	return distances, nil
}
