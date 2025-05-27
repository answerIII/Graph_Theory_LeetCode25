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
	"strings"
	"sync"
)

func (g *Graph) SelectRandomNodes(nodesN int) ([]Node, error) {
	nodesLen := len(g.Nodes)
	if nodesN < 0 || nodesN > nodesLen {
		return nil, fmt.Errorf("can't select %d nodes from graph with %d nodes", nodesN, nodesLen)
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

func (g *Graph) SelectHighestDegree(nodesN int) ([]Node, error) {
	if nodesN < 0 || nodesN > len(g.Nodes) {
		return nil, fmt.Errorf("can't select %d nodes from graph with %d nodes", nodesN, len(g.Nodes))
	}
	nodes := g.GetNodesSlice()

	sort.Slice(nodes, func(i, j int) bool {
		return g.DegreeNode(nodes[i]) > g.DegreeNode(nodes[j])
	})

	return nodes[:nodesN], nil
}

func (g *Graph) SelectBestCoverage(nodesN int) ([]Node, error) {
	if nodesN < 0 || nodesN > len(g.Nodes) {
		return nil, fmt.Errorf("can't select %d nodes from graph with %d nodes", nodesN, len(g.Nodes))
	}

	type pair struct {
		first  Node
		second Node
	}

	used := make(map[pair]bool)
	nodes := make([]Node, 0, nodesN)
	nodesLen := len(g.Nodes)

	const PATHS_COUNT = 500

	var mu sync.Mutex
	wp := workerpool.NewWorkerPool(runtime.NumCPU(), PATHS_COUNT)
	defer wp.Shutdown()

	for len(nodes) < nodesN {
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
					g,
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
					return fmt.Errorf("error while finding shortest path: %v\n", err)
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

func (g *Graph) PrecomputeLandmarks(
	landmarkFilePath string,
	selectFunction func(*Graph, int) ([]Node, error),
	nodesN int,
) error {
	landmarks, err := selectFunction(g, nodesN)
	if err != nil {
		return errors.New("can't select nodes for landmarks")
	}

	landmarkFile, err := os.Create(landmarkFilePath)
	if err != nil {
		return errors.New("can't create landmark file")
	}
	defer landmarkFile.Close()

	_, err = landmarkFile.WriteString(strconv.Itoa(len(g.Nodes)) + "\n")
	if err != nil {
		return err
	}

	for _, u := range landmarks {
		dists, err := BFS(g, []Node{u}, nil, nil, nil)

		if err != nil {
			return errors.New("can't calculate distances for node " + strconv.Itoa(int(u)) + "\n")
		}
		// landmarkFile.WriteString(strconv.Itoa(int(u)) + "\n")
		for _, v := range g.GetNodesSlice() {
			if value, has := dists[v]; has {
				_, err = landmarkFile.WriteString(strconv.Itoa(value) + "\n")
				if err != nil {
					return err
				}
			} else {
				_, err = landmarkFile.WriteString(strconv.Itoa(-1) + "\n")
				if err != nil {
					return err
				}
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
		return -1, fmt.Errorf("there is no node %d in graph", s)
	}
	if t < 0 || t > offset {
		return -1, fmt.Errorf("there is no node %d in graph", t)
	}

	if s > t {
		s, t = t, s
	}
	if err != nil {
		return -1, err
	}

	offsetS := s
	offsetT := t - offsetS
	offsetGlobal := offset - offsetT - 1

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

func (g *Graph) PrecomputeLandmarksWithPaths(
	landmarkFilePath string,
	selectFunction func(*Graph, int) ([]Node, error),
	nodesN int,
) error {
	landmarks, err := selectFunction(g, nodesN)
	if err != nil {
		return errors.New("can't select nodes for landmarks")
	}

	landmarkFile, err := os.Create(landmarkFilePath)
	if err != nil {
		return errors.New("can't create landmark file")
	}
	defer landmarkFile.Close()

	_, err = landmarkFile.WriteString(strconv.Itoa(len(g.Nodes)) + "\n")
	if err != nil {
		return err
	}

	for _, u := range landmarks {
		parents := make(map[Node]Node)
		dists, err := BFS(
			g,
			[]Node{u},
			nil,
			func(node, parent Node, dist int) {
				parents[node] = parent
			},
			nil)

		if err != nil {
			return errors.New("can't calculate distances for node " + strconv.Itoa(int(u)) + "\n")
		}
		landmarkFile.WriteString(strconv.Itoa(int(u)) + "\n")
		for _, v := range g.GetNodesSlice() {
			if _, has := dists[v]; has {
				node := v
				for node != u {
					_, err = landmarkFile.WriteString(strconv.Itoa(int(node)) + " ")
					if err != nil {
						return err
					}
					node = parents[node]
				}
			} else {
				_, err = landmarkFile.WriteString(strconv.Itoa(-1))
				if err != nil {
					return err
				}
			}
			_, err = landmarkFile.WriteString("\n")
			if err != nil {
				return err
			}
		}
	}

	return nil
}

func (g *Graph) LandmarkShortcut(landmarkFilePath string, s, t int) (int, error) {
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
	if err != nil {
		return -1, err
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

		if err != nil {
			return -1, err
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
					if g.HasEdge(Node(sPath[i]), Node(tPath[j])) {
						if i+1+j < dist {
							dist = i + 1 + j
						}
					}
				}
			}
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
