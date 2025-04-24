package main

import "fmt"

func main() {
	connections := [][]int{{0, 1}, {1, 2}, {2, 0}, {1, 3}}
	fmt.Println(criticalConnections(4, connections))
}

func criticalConnections(n int, connections [][]int) [][]int {
	adjList := make([][]int, n)
	for _, edge := range connections {
		adjList[edge[0]] = append(adjList[edge[0]], edge[1])
		adjList[edge[1]] = append(adjList[edge[1]], edge[0])
	}
	timer := 1
	bridges := make([][]int, 0)

	tin := make([]int, n)
	fup := make([]int, n)
	var dfs func(int, int)

	dfs = func(u int, p int) {
		tin[u], fup[u] = timer, timer
		timer++

		for _, v := range adjList[u] {
			if v == p {
				continue
			}
			if tin[v] != 0 {
				fup[u] = min(fup[u], tin[v])
			} else {
				dfs(v, u)
				fup[u] = min(fup[u], fup[v])
				if tin[u] < fup[v] {
					bridges = append(bridges, []int{u, v})
				}
			}
		}
	}

	dfs(0, -1)
	return bridges
}
