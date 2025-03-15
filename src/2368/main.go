package _2368

func reachableNodes(n int, edges [][]int, restricted []int) int {
	counter := 0

	graph := make(map[int][]int)
	for _, edge := range edges {
		graph[edge[0]] = append(graph[edge[0]], edge[1])
		graph[edge[1]] = append(graph[edge[1]], edge[0])
	}

	visited := make([]bool, n)
	for _, node := range restricted {
		visited[node] = true
	}

	nodes := []int{0}
	for len(nodes) > 0 {
		var newNodes []int
		for _, v := range nodes {
			counter += 1
			visited[v] = true
			for _, node := range graph[v] {
				if visited[node] {
					continue
				}
				newNodes = append(newNodes, node)
			}
		}
		nodes = newNodes
	}

	return counter
}
