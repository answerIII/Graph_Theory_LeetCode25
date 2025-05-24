package graph

import (
	"bufio"
	"errors"
	"fmt"
	"graph_theory/workerpool"
	"math"
	"math/rand"
	"os"
	"runtime"
	"slices"
	"sort"
	"strconv"
	"sync"
)

func (graph *Graph) SelectRandomNodes(nodesN int) ([]Node, error) {
	nodesLen := len(graph.Nodes)
	if nodesN < 0 || nodesN > nodesLen {
		return nil, fmt.Errorf("Can't select %d nodes from graph with %d nodes", nodesN, nodesLen)
	}
	used := make(map[int]bool)
	randomNodes := make([]Node, 0, nodesN)

	for len(randomNodes) < nodesN {
		n := rand.Intn(nodesLen)
		if !used[n] {
			used[n] = true
			randomNodes = append(randomNodes, Node(n))
		}
	}

	return randomNodes, nil
}

func (graph *Graph) SelectHighestDegree(nodesN int) ([]Node, error) {
	if nodesN < 0 || nodesN > len(graph.Nodes) {
		return nil, fmt.Errorf("Can't select %d nodes from graph with %d nodes", nodesN, len(graph.Nodes))
	}
	nodes := graph.getNodesSlice()

	sort.Slice(nodes, func(i, j int) bool {
		return graph.DegreeNode(nodes[i]) > graph.DegreeNode(nodes[j])
	})

	return nodes[:nodesN], nil
}

func (graph *Graph) SelectBestCoverage(nodesN int) ([]Node, error) {
	if nodesN < 0 || nodesN > len(graph.Nodes) {
		return nil, fmt.Errorf("Can't select %d nodes from graph with %d nodes", nodesN, len(graph.Nodes))
	}

	type pair struct {
		first  Node
		second Node
	}

	used := make(map[pair]bool)
	nodes := make([]Node, 0, nodesN)
	nodesLen := len(graph.Nodes)

	const PATHS_COUNT = 500

	for len(nodes) < nodesN {
		wp := workerpool.NewWorkerPool(runtime.NumCPU(), PATHS_COUNT)
		defer wp.Shutdown()

		var mu sync.Mutex

		destinations := make([]pair, 0, PATHS_COUNT)
		paths := make([][]Node, 0, PATHS_COUNT)

		for len(destinations) < PATHS_COUNT {
			f := rand.Intn(nodesLen)
			s := rand.Intn(nodesLen)
			if f == s {
				continue
			}
			if f > s {
				f, s = s, f
			}
			p := pair{Node(f), Node(s)}
			if !used[p] {
				used[p] = true
				destinations = append(destinations, p)
			}
		}

		for _, d := range destinations {
			wp.Submit(func() error {
				parents := make(map[Node]Node)
				_, err := BFS(
					graph,
					[]Node{d.first},
					nil,
					func(node, parent Node, dist int) {
						parents[node] = parent
					},
					func(node Node, dist int) bool {
						return node == d.second
					},
				)
				if err != nil {
					return fmt.Errorf("Error while finding shortest path: %v\n", err)
				}

				path := []Node{d.second}
				n := d.second
				for n != d.first {
					n = parents[n]
					path = append(path, n)
				}

				mu.Lock()
				paths = append(paths, path)
				mu.Unlock()

				return nil
			})
		}

		wp.Wait()

		counter := make(map[Node]int)

		for _, path := range paths {
			for _, node := range path {
				counter[node]++
			}
		}

		for len(paths) > 0 && len(nodes) < nodesN {
			var maxNode Node
			maxValue := -1

			for k, v := range counter {
				if v > maxValue {
					maxValue = v
					maxNode = k
				}
			}

			nodes = append(nodes, maxNode)

			for idx, path := range paths {
				if slices.Contains(path, maxNode) {
					for _, node := range path {
						counter[node]--
					}
				}
				paths = slices.Delete(paths, idx, idx+1)
			}
		}
	}

	return nodes, nil
}

func (graph *Graph) PrecomputeLandmarks(
	landmarkFilePath string,
	selectFunction func(*Graph, int) ([]Node, error),
	nodesN int,
) error {
	landmarks, err := selectFunction(graph, nodesN)
	if err != nil {
		return errors.New("Can't select nodes for landmarks")
	}

	landmarkFile, err := os.Create(landmarkFilePath)
	if err != nil {
		return errors.New("Can't create landmark file")
	}
	defer landmarkFile.Close()

	landmarkFile.WriteString(strconv.Itoa(len(graph.Nodes)) + "\n")

	for _, u := range landmarks {
		dists, err := BFS(graph, []Node{u}, nil, nil, nil)

		if err != nil {
			return errors.New("Can't calculate distances for node " + strconv.Itoa(int(u)) + "\n")
		}
		// landmarkFile.WriteString(strconv.Itoa(int(u)) + "\n")
		for v := range graph.Nodes {
			if value, has := dists[v]; has {
				landmarkFile.WriteString(strconv.Itoa(value) + "\n")
			} else {
				landmarkFile.WriteString(strconv.Itoa(-1) + "\n")
			}
		}
	}

	return nil
}

func LandmarkBasic(landmarkFilePath string, s, t int) (int, error) {
	landmarkFile, err := os.Open(landmarkFilePath)
	if err != nil {
		return -1, err
	}
	defer landmarkFile.Close()

	dist := math.MaxInt

	scanner := bufio.NewScanner(landmarkFile)

	scanner.Scan()

	offset, err := strconv.Atoi(scanner.Text())

	if s < 0 || s > offset {
		return -1, fmt.Errorf("There is no node %d in graph", s)
	}
	if t < 0 || t > offset {
		return -1, fmt.Errorf("There is no node %d in graph", t)
	}

	if s > t {
		s, t = t, s
	}
	if err != nil {
		return -1, err
	}

	offsetS := s + 1
	offsetT := t + 1 - offsetS
	offsetGlobal := offset + 1 - offsetT

	for scanner.Scan() {
		for range offsetS {
			scanner.Scan()
		}

		sDist, err := strconv.Atoi(scanner.Text())

		if err != nil {
			return -1, err
		}

		for range offsetT {
			scanner.Scan()
		}

		tDist, err := strconv.Atoi(scanner.Text())
		if sDist != -1 && tDist != -1 {
			dist = min(dist, sDist+tDist)
		}

		if err != nil {
			return -1, err
		}

		for range offsetGlobal {
			scanner.Scan()
		}
	}
	return dist, nil
}
