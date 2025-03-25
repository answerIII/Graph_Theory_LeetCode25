package main

import "fmt"

func main() {
	fmt.Println(findTheCity(5, [][]int{{0, 1, 2}, {0, 4, 8}, {1, 2, 3}, {1, 4, 2}, {2, 3, 1}, {3, 4, 1}}, 2))
}

type Edge struct {
	To     int
	Weight int
}

func findTheCity(n int, edges [][]int, distanceThreshold int) int {
	adjacencyList := make([][]Edge, n)
	for _, edge := range edges {
		adjacencyList[edge[0]] = append(adjacencyList[edge[0]], Edge{To: edge[1], Weight: edge[2]})
		adjacencyList[edge[1]] = append(adjacencyList[edge[1]], Edge{To: edge[0], Weight: edge[2]})
	}

	ans := 0
	minNumberReachableCities := n
	for v := range adjacencyList {
		curNumberReachableCities := numberReachableCities(v, n, distanceThreshold, adjacencyList)
		if curNumberReachableCities <= minNumberReachableCities {
			ans = v
			minNumberReachableCities = curNumberReachableCities
		}
	}

	return ans
}

func numberReachableCities(v, n, distanceThreshold int, adjacencyList [][]Edge) int {
	dist := make([]int, n)
	ans := 0
	isCalculated := make([]bool, n)

	for {
		minV := v
		for j := range dist {
			if dist[j] != 0 && !isCalculated[j] && (dist[j] < dist[minV] || minV == v) {
				minV = j
			}
		}
		if isCalculated[minV] {
			return ans - 1
		}
		isCalculated[minV] = true
		ans++
		for _, edge := range adjacencyList[minV] {
			if (dist[minV]+edge.Weight < dist[edge.To] || dist[edge.To] == 0) && dist[minV]+edge.Weight <= distanceThreshold {
				dist[edge.To] = dist[minV] + edge.Weight
			}
		}
	}
}
