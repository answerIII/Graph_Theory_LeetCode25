package graph

import (
	"fmt"
	"math"
	"os"
)

func (g *Graph) DegreeNode(n Node) int {
	return len(g.Adj[n])
}

func (g *Graph) NumberOfNodes() int {
	return len(g.Nodes)
}

func (g *Graph) NumberOfEdges() int {
	edges := 0
	for node := range g.Nodes {
		neighbors := g.GetNeighborsMap(node)
		edges += len(neighbors)
	}
	if !g.Directed {
		edges /= 2
	}
	return edges
}

func (g *Graph) Density() float64 {
	n := g.NumberOfNodes()
	if n <= 1 {
		return 0.0
	}
	e := g.NumberOfEdges()
	if g.Directed {
		return float64(e) / float64(n*(n-1))
	}
	return float64(e*2) / float64(n*(n-1))
}

func (g *Graph) ProcessNodesDegrees(filePath string) (minDeg int, avgDeg float64, maxDeg int, err error) {
	degreesFrequency := make(map[int]int)

	var sum int64
	minDegree := math.MaxInt32
	maxDegree := 0

	for node := range g.Nodes {
		degree := len(g.Adj[node])
		if degree < minDegree {
			minDegree = degree
		}
		if degree > maxDegree {
			maxDegree = degree
		}
		sum += int64(degree)
		degreesFrequency[degree]++
	}

	if filePath != "" {
		file, err := os.Create(filePath)
		if err != nil {
			return 0, 0, 0, err
		}
		defer file.Close()

		_, err = fmt.Fprint(file, "# degree frequency\n")
		if err != nil {
			return 0, 0, 0, err
		}
		for degree, freq := range degreesFrequency {
			_, err := fmt.Fprintf(file, "%d %d\n", degree, freq)
			if err != nil {
				return 0, 0, 0, err
			}
		}
	}

	avgDegree := float64(sum) / float64(len(g.Nodes))
	return minDegree, avgDegree, maxDegree, nil
}
