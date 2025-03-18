package _3310

func markSuspicious(graph [][]int, k int, suspicious []bool) {
	nodes := []int{k}
	for len(nodes) > 0 {
		var newNodes []int
		for _, v := range nodes {
			suspicious[v] = true
			for _, node := range graph[v] {
				if !suspicious[node] {
					newNodes = append(newNodes, node)
				}
			}
		}
		nodes = newNodes
	}
}

func remainingMethods(n int, k int, invocations [][]int) []int {
	graph := make([][]int, n)
	for _, inv := range invocations {
		graph[inv[0]] = append(graph[inv[0]], inv[1])
	}

	suspicious := make([]bool, n)
	markSuspicious(graph, k, suspicious)

	canRemoveSusGroup := true
	for _, inv := range invocations {
		if !suspicious[inv[0]] && suspicious[inv[1]] {
			canRemoveSusGroup = false
			break
		}
	}
	var res []int

	for i := 0; i < n; i++ {
		if canRemoveSusGroup {
			if !suspicious[i] {
				res = append(res, i)
			}
		} else {
			res = append(res, i)
		}
	}

	return res
}
