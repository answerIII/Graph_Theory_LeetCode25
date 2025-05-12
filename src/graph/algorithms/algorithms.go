package graph

func DFS[T comparable](
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

				for v := range edges[u] {
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

// BFS finds the farthest node from the starting node in an undirected graph.
func BFS[T comparable](
	nodes []T,
	edges map[T]map[T]struct{},
	used map[T]struct{},
) (T, int, error) {
	if used == nil {
		used = make(map[T]struct{})
	}

	maxDistanceNode := struct {
		node     T
		distance int
	}{nodes[0], 0}

	queue := []T{nodes[0]}
	used[nodes[0]] = struct{}{}
	distances := make(map[T]int)
	distances[nodes[0]] = 0

	for len(queue) > 0 {
		u := queue[0]
		queue = queue[1:]

		for neighbor := range edges[u] {
			if _, ok := used[neighbor]; !ok {
				used[neighbor] = struct{}{}
				queue = append(queue, neighbor)
				distances[neighbor] = distances[u] + 1
				if distances[neighbor] > maxDistanceNode.distance {
					maxDistanceNode.node = neighbor
					maxDistanceNode.distance = distances[neighbor]
				}
			}
		}
	}

	return maxDistanceNode.node, maxDistanceNode.distance, nil
}
