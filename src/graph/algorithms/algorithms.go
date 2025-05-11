package graph

func DFS[T comparable](
	nodes map[T]struct{},
	edges map[T]map[T]struct{},
	used map[T]struct{},
	onIn, onOut func(T),
) [][]T {
	if used == nil {
		used = make(map[T]struct{})
	}

	var components [][]T

	for node := range nodes {
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
