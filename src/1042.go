package main

import "fmt"

func main() {
	fmt.Println(gardenNoAdj(6, [][]int{{6, 4}, {6, 1}, {3, 1}, {4, 5}, {2, 1}, {5, 6}, {5, 2}}))
}

func gardenNoAdj(n int, paths [][]int) []int {
	adjacencyList := make([][]int, n)
	for _, edge := range paths {
		adjacencyList[edge[0]-1] = append(adjacencyList[edge[0]-1], edge[1]-1)
		adjacencyList[edge[1]-1] = append(adjacencyList[edge[1]-1], edge[0]-1)
	}

	res := make([]int, n)
	for v := range res {
		isBusy := [5]bool{}
		for _, adjV := range adjacencyList[v] {
			isBusy[res[adjV]] = true
		}
		for plant := 1; plant < 5; plant++ {
			if !isBusy[plant] {
				res[v] = plant
				break
			}
		}
	}

	return res
}
