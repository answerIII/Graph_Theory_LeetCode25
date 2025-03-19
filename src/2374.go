package main

import "fmt"

func main() {
	fmt.Println(edgeScore([]int{1, 0, 0, 0, 0, 7, 7, 5}))
}

func edgeScore(edges []int) int {
	nodeWithHighestScore := 0
	nodeToScore := make([]int, len(edges))

	for node, edge := range edges {
		nodeToScore[edge] += node
		if nodeToScore[nodeWithHighestScore] < nodeToScore[edge] {
			nodeWithHighestScore = edge
		} else if nodeToScore[nodeWithHighestScore] == nodeToScore[edge] && edge < nodeWithHighestScore {
			nodeWithHighestScore = edge
		}
	}

	return nodeWithHighestScore
}
