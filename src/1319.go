func makeConnected(n int, connections [][]int) int {
	m := len(connections)
	if m < n-1 {
		return -1
	}

	edges := make([][]int, n)
	for _, edge := range connections {
		edges[edge[0]] = append(edges[edge[0]], edge[1])
		edges[edge[1]] = append(edges[edge[1]], edge[0])
	}

	visited := make([]bool, n)
	bfs_q := make([]int, n)
	start_q := 0
	end_q := 0
	result := -1

	for i := range n {
		if visited[i] {
			continue
		}
		result++
		bfs_q[end_q] = i
		end_q++
		visited[i] = true

		for start_q < end_q {
			for _, v := range edges[bfs_q[start_q]] {
				if !visited[v] {
					bfs_q[end_q] = v
					visited[v] = true
					end_q++
				}
			}
			// visited[bfs_q[start_q]] = true
			start_q++
		}

	}
	return result

}