package _1557

func findSmallestSetOfVertices(n int, edges [][]int) []int {
	var hasIncoming = make([]bool, n)
	for _, edge := range edges {
		hasIncoming[edge[1]] = true
	}
	var result = make([]int, 0, n)

	for i := 0; i < n; i++ {
		if !hasIncoming[i] {
			result = append(result, i)
		}
	}

	return result
}
