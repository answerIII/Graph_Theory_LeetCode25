package graph

import (
	"graph_theory/workerpool"
	"log"
	"math"
	"runtime"
	"sort"
	"sync"
)

func (g *Graph) GetDiameterDoubleSweep(randomNode Node) int {
	source := []Node{randomNode}

	farNode := struct {
		node     Node
		distance int
	}{randomNode, 0}

	updFarNode := func(node, parent Node, dist int) {
		if dist > farNode.distance {
			farNode.node = node
			farNode.distance = dist
		}
	}

	_, err := BFS(g, source, nil, updFarNode, nil)
	if err != nil {
		log.Fatalf("Error in BFS: %v\n", err)
	}

	source[0] = farNode.node
	farNode.distance = 0
	_, err = BFS(g, source, nil, updFarNode, nil)
	if err != nil {
		log.Fatalf("Error in BFS: %v\n", err)
	}

	return farNode.distance
}

func (g *Graph) GetDistancePercentile(
	component []Node,
	percentile float64,
	sampleN int,
) (float64, error) {
	sampleNodes := generateSampleNodes(component, sampleN)
	wp := workerpool.NewWorkerPool(runtime.NumCPU(), len(sampleNodes))
	defer wp.Shutdown()

	var mu sync.Mutex
	dists := make([]int, 0, len(sampleNodes))

	for _, p := range sampleNodes {
		p := p
		wp.Submit(func() error {
			var dist int
			_, err := BFS(
				g,
				[]Node{p[0]},
				nil,
				nil,
				func(n Node, d int) bool {
					if n == p[1] {
						dist = d
						return true
					}
					return false
				},
			)
			if err == nil {
				mu.Lock()
				dists = append(dists, dist)
				mu.Unlock()
			}
			return nil
		})
	}

	wp.Wait()

	sort.Slice(dists, func(i, j int) bool {
		return dists[i] < dists[j]
	})

	sampleN = min(sampleN, len(sampleNodes))
	i := percentile*float64(sampleN+1) - 1
	floor_i := int(math.Floor(i))
	ans := float64(dists[floor_i]) + (i-float64(floor_i))*float64(dists[floor_i+1]-dists[floor_i])

	return ans, nil
}
