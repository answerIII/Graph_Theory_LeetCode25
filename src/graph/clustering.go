package graph

import (
	"fmt"
	"graph_theory/workerpool"
	"runtime"
	"slices"
	"sync"
	"sync/atomic"
)

func (g *Graph) GetLocalClusteringCoefficient(node Node) (float64, error) {

	neighbors := g.Adj.neighbors(node)
	k := len(neighbors)
	if k < 2 {
		return 0, nil
	}
	edges := 0

	for u := 0; u < k; u++ {
		for v := u + 1; v < k; v++ {
			if g.HasEdge(neighbors[v], neighbors[u]) {
				edges++
			}
		}
	}

	return float64(2*edges) / float64(k*(k-1)), nil
}

func (g *Graph) GetAverageClusteringCoefficient(nodes []Node) (float64, error) {
	if g.Directed {
		return 0, fmt.Errorf("graph must be undirected")
	}

	if nodes == nil {
		nodes = g.getNodesSlice()
	}

	var sum float64
	var mu sync.Mutex
	wp := workerpool.NewWorkerPool(runtime.NumCPU(), len(nodes))
	defer wp.Shutdown()

	for _, node := range nodes {
		node := node
		wp.Submit(func() error {
			loc, err := g.GetLocalClusteringCoefficient(node)
			if err != nil {
				return err
			}
			mu.Lock()
			sum += loc
			mu.Unlock()
			return nil
		})
	}

	wp.Wait()

	return sum / float64(len(nodes)), nil
}

func (g *Graph) GetGlobalClusteringCoefficient(triangles int64) (float64, error) {
	if g.Directed {
		return 0, fmt.Errorf("graph must be undirected")
	}

	if triangles < 0 {
		var err error
		triangles, err = g.TrianglesNumber()
		if err != nil {
			return 0, err
		}
	}

	sum := 0.0
	for node := range g.Nodes {
		l := len(g.Adj.neighbors(node))
		sum += float64(l * (l - 1) / 2)
	}
	return float64(3*triangles) / sum, nil
}

func (g *Graph) TrianglesNumber() (int64, error) {
	if g.Directed {
		return 0, fmt.Errorf("graph must be undirected")
	}
	var triangles atomic.Int64

	nodes := make([]Node, 0, len(g.Nodes))
	for node := range g.Nodes {
		nodes = append(nodes, node)
	}

	workers := runtime.NumCPU()
	chunkSize := (len(nodes) + workers - 1) / workers

	wp := workerpool.NewWorkerPool(workers, len(nodes)/chunkSize+1)
	defer wp.Shutdown()

	for i := 0; i < len(nodes); i += chunkSize {
		start := i
		end := i + chunkSize
		if end > len(nodes) {
			end = len(nodes)
		}
		wp.Submit(func() error {
			var local int64
			for j := start; j < end; j++ {
				node1 := nodes[j]
				neighbors := g.Adj.neighbors(node1)
				for _, node2 := range neighbors {
					if node2 > node1 {
						for _, node3 := range g.Adj.neighbors(node2) {
							if _, ok := slices.BinarySearch(neighbors, node3); ok && node3 > node2 {
								local++
							}
						}
					}
				}
			}
			triangles.Add(local)
			return nil
		})
	}

	wp.Wait()

	return triangles.Load(), nil
}
