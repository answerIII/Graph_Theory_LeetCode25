func minMalwareSpread(graph [][]int, initial []int) int {

	n := len(graph)
	isInitial := make([]bool, n)
	infectedTimes := make([]int, n)
	startInfected := make([]int, n)
	ans := initial[0]
	for i := range n {
		startInfected[i] = -1
		isInitial[i] = false
		graph[i][i] = 0
	}
	for _, val := range initial {
		isInitial[val] = true
		infectedTimes[val] = 301
	}

	bfsQueue := make([]int, n)
	first, last := 0, 1
	for _, start := range initial {
		bfsQueue[0] = start

		for first < last {
			for vertex, edge := range graph[bfsQueue[first]] {
				if edge == 1 && !isInitial[vertex] && infectedTimes[vertex] < infectedTimes[bfsQueue[first]] {
					bfsQueue[last] = vertex
					infectedTimes[vertex]++
					last++
					startInfected[vertex] = start
				}
			}
			first++
		}

		first, last = 0, 1
	}

	for _, j := range initial {
		infectedTimes[j] = 0
	}

	for i := range n {
		if !isInitial[i] && infectedTimes[i] == 1 {
			infectedTimes[startInfected[i]]++
		}
	}

	for _, j := range initial {
		if infectedTimes[j] > infectedTimes[ans] || infectedTimes[j] == infectedTimes[ans] && j < ans {
			ans = j
		}
	}

	return ans

}