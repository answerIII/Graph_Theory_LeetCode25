package graph

import (
	"golang.org/x/exp/constraints"
	"sort"
)

func DFS[T constraints.Ordered](
	nodes []T,
	edges map[T]map[T]struct{},
	used map[T]struct{},
	onIn, onOut func(T),
) [][]T {
	if used == nil {
		used = make(map[T]struct{})
	}

	var components [][]T

	for _, node := range nodes {
		if _, ok := used[node]; ok {
			continue
		}

		var stack []struct {
			node        T
			isProcessed bool
		}
		var path []T

		stack = append(stack, struct {
			node        T
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
					node        T
					isProcessed bool
				}{u, true})

				neighbors := make([]T, 0, len(edges[u]))
				for v := range edges[u] {
					neighbors = append(neighbors, v)
				}
				sort.Slice(neighbors, func(i, j int) bool {
					return neighbors[i] < neighbors[j]
				})

				for _, v := range neighbors {
					if _, ok := used[v]; !ok {
						stack = append(stack, struct {
							node        T
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

func BFS[T constraints.Ordered](
	nodes []T,
	edges map[T]map[T]struct{},
	used map[T]struct{},
	onVisit func(T, int),
	stopCond func(T, int) bool,
) (map[T]int, error) {
	if used == nil {
		used = make(map[T]struct{})
	}

	distances := make(map[T]int)

	for _, node := range nodes {
		distances[node] = 0
		queue := []T{node}
		used[node] = struct{}{}

		for len(queue) > 0 {
			u := queue[0]
			if onVisit != nil {
				onVisit(u, distances[u])
			}
			if stopCond != nil && stopCond(u, distances[u]) {
				return distances, nil
			}
			queue = queue[1:]

			neighbors := make([]T, 0, len(edges[u]))
			for v := range edges[u] {
				neighbors = append(neighbors, v)
			}
			sort.Slice(neighbors, func(i, j int) bool {
				return neighbors[i] < neighbors[j]
			})

			for _, neighbor := range neighbors {
				if _, ok := used[neighbor]; !ok {
					used[neighbor] = struct{}{}
					queue = append(queue, neighbor)
					distances[neighbor] = distances[u] + 1
				}
			}
		}
	}

	return distances, nil
}
