package landmarkAlgo

import (
	"bufio"
	"encoding/binary"
	"errors"
	"fmt"
	"graph_theory/graph"
	"graph_theory/workerpool"
	"math"
	"math/rand"
	"os"
	"runtime"
	"slices"
	"sort"
	"strconv"
	"strings"
	"sync"
)

func SelectRandomNodes(g *graph.Graph, nodesN int) ([]graph.Node, error) {
	nodesLen := len(g.Nodes)
	if nodesN < 0 || nodesN > nodesLen {
		return nil, fmt.Errorf("can't select %d nodes from graph with %d nodes", nodesN, nodesLen)
	}
	used := make(map[int]bool)
	randomNodes := make([]graph.Node, 0, nodesN)

	for len(randomNodes) < nodesN {
		n := rand.Intn(nodesLen)
		if !used[n] {
			used[n] = true
			randomNodes = append(randomNodes, graph.Node(n))
		}
	}

	return randomNodes, nil
}

func SelectHighestDegree(g *graph.Graph, nodesN int) ([]graph.Node, error) {
	if nodesN < 0 || nodesN > len(g.Nodes) {
		return nil, fmt.Errorf("can't select %d nodes from graph with %d nodes", nodesN, len(g.Nodes))
	}
	nodes := g.GetNodesSlice()

	sort.Slice(nodes, func(i, j int) bool {
		return g.DegreeNode(nodes[i]) > g.DegreeNode(nodes[j])
	})

	return nodes[:nodesN], nil
}

func SelectBestCoverage(g *graph.Graph, nodesN int) ([]graph.Node, error) {
	if nodesN < 0 || nodesN > len(g.Nodes) {
		return nil, fmt.Errorf("can't select %d nodes from graph with %d nodes", nodesN, len(g.Nodes))
	}

	type pair struct {
		first  graph.Node
		second graph.Node
	}

	used := make(map[pair]bool)
	nodes := make([]graph.Node, 0, nodesN)
	nodesLen := len(g.Nodes)

	const PATHS_COUNT = 500

	var mu sync.Mutex
	wp := workerpool.NewWorkerPool(runtime.NumCPU(), PATHS_COUNT)
	defer wp.Shutdown()

	for len(nodes) < nodesN {
		destinations := make([]pair, 0, PATHS_COUNT)
		paths := make([][]graph.Node, 0, PATHS_COUNT)

		for len(destinations) < PATHS_COUNT {
			f := rand.Intn(nodesLen)
			s := rand.Intn(nodesLen)
			if f == s {
				continue
			}
			if f > s {
				f, s = s, f
			}
			p := pair{graph.Node(f), graph.Node(s)}
			if !used[p] {
				used[p] = true
				destinations = append(destinations, p)
			}
		}

		for _, d := range destinations {
			wp.Submit(func() error {
				parents := make(map[graph.Node]graph.Node)
				_, err := graph.BFS(
					g,
					[]graph.Node{d.first},
					nil,
					func(node, parent graph.Node, dist int) {
						parents[node] = parent
					},
					func(node graph.Node, dist int) bool {
						return node == d.second
					},
				)
				if err != nil {
					return fmt.Errorf("error while finding shortest path: %v\n", err)
				}

				path := []graph.Node{d.second}
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

		counter := make(map[graph.Node]int)

		for _, path := range paths {
			for _, node := range path {
				counter[node]++
			}
		}

		for len(paths) > 0 && len(nodes) < nodesN {
			var maxNode graph.Node
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

func LandmarkBasic(landmarkFilePath string, s, t int32) (int, error) {
	file, err := os.Open(landmarkFilePath)
	if err != nil {
		return -1, err
	}
	defer file.Close()

	var numNodes int32
	var numLandmarks int32

	const headerSize = 2
	err = binary.Read(file, binary.LittleEndian, &numNodes)
	if err != nil {
		return 0, err
	}
	err = binary.Read(file, binary.LittleEndian, &numLandmarks)
	if err != nil {
		return 0, err
	}

	if s < 0 || s >= numNodes {
		return 0, fmt.Errorf("there is no node %d in graph", s)
	}
	if t < 0 || t >= numNodes {
		return 0, fmt.Errorf("there is no node %d in graph", t)
	}

	if s > t {
		s, t = t, s
	}

	const int32Size = 4
	dist := int32(math.MaxInt32)

	for i := int64(0); i < int64(numLandmarks); i++ {
		baseOffset := headerSize*int32Size + i*int64(numNodes)*int32Size

		_, err = file.Seek(baseOffset+int64(s)*int32Size, 0)
		if err != nil {
			return 0, err
		}
		var sDist int32
		err = binary.Read(file, binary.LittleEndian, &sDist)
		if err != nil {
			return 0, err
		}

		_, err = file.Seek(baseOffset+int64(t)*int32Size, 0)
		if err != nil {
			return 0, err
		}
		var tDist int32
		err = binary.Read(file, binary.LittleEndian, &tDist)
		if err != nil {
			return 0, err
		}

		if sDist != -1 && tDist != -1 {
			dist = min(dist, sDist+tDist)
		}
	}

	if dist == math.MaxInt32 {
		return 0, errors.New("no path found via landmarks")
	}

	return int(dist), nil
}

func LandmarkShortcut(g *graph.Graph, landmarkFilePath string, s, t int) (int, error) {
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
		return -1, fmt.Errorf("there is no node %d in graph", s)
	}
	if t < 0 || t > offset {
		return -1, fmt.Errorf("there is no node %d in graph", t)
	}

	if s > t {
		s, t = t, s
	}

	offsetS := s + 1
	offsetT := t - offsetS + 1
	offsetGlobal := offset - offsetT - 1

	for scanner.Scan() {
		landmark := strings.TrimSuffix(scanner.Text(), "\n")

		for range offsetS {
			scanner.Scan()
		}

		sStringPath := strings.Split(scanner.Text(), " ")
		sStringPath[len(sStringPath)-1] = landmark
		sPath := make([]int, len(sStringPath))
		for idx, val := range sStringPath {
			if sPath[idx], err = strconv.Atoi(val); err != nil {
				return -1, err
			}
		}

		for range offsetT {
			scanner.Scan()
		}

		tStringPath := strings.Split(scanner.Text(), " ")
		tStringPath[len(tStringPath)-1] = landmark
		tPath := make([]int, len(tStringPath))
		for idx, val := range tStringPath {
			if tPath[idx], err = strconv.Atoi(val); err != nil {
				return -1, err
			}
		}

		if sPath[0] != -1 && tPath[0] != -1 {
			sLCAIdx := 0
			for !slices.Contains(tPath, sPath[sLCAIdx]) {
				sLCAIdx++
			}
			tLCAIdx := slices.Index(tPath, sPath[sLCAIdx])
			for i := range sLCAIdx {
				for j := range tLCAIdx {
					if g.HasEdge(graph.Node(sPath[i]), graph.Node(tPath[j])) {
						if i+1+j < dist {
							dist = i + 1 + j
						}
					}
				}
			}
		}

		for range offsetGlobal {
			scanner.Scan()
		}
	}
	return dist, nil
}
