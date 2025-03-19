func findChampion(n int, edges [][]int) int {
	degree_in := make([]int, n)
	for _, edge := range edges {
		degree_in[edge[1]]++
	}
	ans := -2
	for idx, degree := range degree_in {
		if degree == 0 && ans == -2 {
			ans = idx
		} else if degree == 0 {
			ans = -1
		}
	}
	return ans
}