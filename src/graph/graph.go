package graph

import (
	"bufio"
	"errors"
	"fmt"
	algo "graph_theory/graph/algorithms"
	"graph_theory/workerpool"
	"log"
	"math"
	"math/rand/v2"
	"os"
	"runtime"
	"sort"
	"strconv"
	"strings"
	"sync"
)

type Node int
type Nodes map[Node]struct{}
type Adjacency map[Node]map[Node]struct{}

type Graph struct {
	Directed bool
	Adj      Adjacency
	Nodes    Nodes
}

func NewGraph(directed bool) *Graph {
	return &Graph{
		Directed: directed,
		Adj:      make(Adjacency),
		Nodes:    make(Nodes),
	}
}

func (g *Graph) adj() Adjacency {
	return g.Adj
}

func (g *Graph) nodes() Nodes {
	return g.Nodes
}

func (g *Graph) nodesSlice() []Node {
	nodes := make([]Node, 0, g.NumberOfNodes())
	for node, _ := range g.Nodes {
		nodes = append(nodes, node)
	}
	sort.Slice(nodes, func(i, j int) bool {
		return nodes[i] < nodes[j]
	})
	return nodes
}

func (g *Graph) AddNode(n Node) {
	if _, exists := g.Nodes[n]; !exists {
		g.Nodes[n] = struct{}{}
		g.Adj[n] = make(Nodes)
	}
}

func (g *Graph) RemoveNode(n Node) {
	if _, exists := g.Nodes[n]; exists {
		delete(g.Nodes, n)
		delete(g.Adj, n)
		for _, neighbors := range g.Adj {
			delete(neighbors, n)
		}
	} else {
		panic(fmt.Sprintf("Node %d does not exist in the graph", n))
	}
}

func (g *Graph) HasNode(n Node) bool {
	_, exists := g.Nodes[n]
	return exists
}

func (g *Graph) AddEdge(u, v Node) {
	if _, ok := g.Nodes[u]; !ok {
		g.AddNode(u)
	}
	if _, ok := g.Nodes[v]; !ok {
		g.AddNode(v)
	}
	if _, ok := g.Adj[u][v]; !ok {
		g.Adj[u][v] = struct{}{}
	}
	if !g.Directed && u != v {
		if _, ok := g.Adj[v][u]; !ok {
			g.Adj[v][u] = struct{}{}
		}
	}
}

func (g *Graph) RemoveEdge(u, v Node) {
	delete(g.Adj[u], v)
	if !g.Directed && u != v {
		delete(g.Adj[v], u)
	}
}

func (g *Graph) HasEdge(u, v Node) bool {
	_, exists := g.Adj[u][v]
	return exists
}

func (g *Graph) Inverted() *Graph {
	inverted := NewGraph(g.Directed)
	for u, neighbors := range g.Adj {
		for v := range neighbors {
			inverted.AddEdge(v, u)
		}
	}
	return inverted
}

func (g *Graph) DegreeNode(n Node) int {
	return len(g.Adj[n])
}

func (g *Graph) NumberOfNodes() int {
	return len(g.Nodes)
}

func (g *Graph) NumberOfEdges() int {
	edges := 0
	for _, neighbors := range g.Adj {
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

func (g *Graph) CastToUndirected() *Graph {
	if !g.Directed {
		panic("Graph is already undirected")
	}
	ug := NewGraph(false)
	for u, neighbors := range g.Adj {
		for v := range neighbors {
			ug.AddEdge(u, v)
			ug.AddEdge(v, u)
		}
	}
	return ug
}

func (g *Graph) FindWCC() ([][]Node, error) {
	if g.Directed {
		return nil, errors.New("can't find WCC in directed graph")
	}
	return algo.DFS(g.nodesSlice(), g.Adj, nil, nil, nil), nil
}

func (g *Graph) FindSCC() ([][]Node, error) {
	if !g.Directed {
		return nil, errors.New("can't find SCC in undirected graph")
	}

	nodes := g.nodesSlice()

	tout := make(map[Node]int)
	time := 0

	onOut := func(node Node) {
		tout[node] = time
		time++
	}

	algo.DFS(nodes, g.Adj, nil, nil, onOut)

	sort.Slice(nodes, func(i, j int) bool {
		return tout[nodes[i]] > tout[nodes[j]]
	})

	inverted := g.Inverted()

	return algo.DFS(nodes, inverted.Adj, nil, nil, nil), nil
}

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

	_, err := algo.BFS(source, g.Adj, nil, updFarNode, nil)
	if err != nil {
		log.Fatalf("Error in BFS: %v\n", err)
	}

	source[0] = farNode.node
	farNode.distance = 0
	_, err = algo.BFS(source, g.Adj, nil, updFarNode, nil)
	if err != nil {
		log.Fatalf("Error in BFS: %v\n", err)
	}

	return farNode.distance
}

func (g *Graph) GetLocalClusteringCoefficient(node Node) (float64, error) {

	neighbours := g.Adj[node]
	k := len(neighbours)
	if k < 2 {
		return 0, nil
	}
	edges := 0

	for u := range neighbours {
		for v := range neighbours {
			if _, exists := g.Adj[v][u]; exists && u > v {
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
		nodes = g.nodesSlice()
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
		l := len(g.Adj[node])
		sum += float64(l * (l - 1) / 2)
	}
	return float64(3*triangles) / sum, nil
}

func generateSampleNodes(
	nodes []Node,
	sampleN int,
) [][]Node {
	sampleN = int(min(int64(sampleN), int64(len(nodes))*int64(len(nodes)-1)))
	type pair struct {
		u, v Node
	}
	sampleNodes := make(map[pair]struct{})
	compLen := len(nodes)

	for len(sampleNodes) < sampleN {
		u := nodes[rand.IntN(compLen)]
		v := nodes[rand.IntN(compLen)]
		if u == v {
			continue
		}
		if u > v {
			u, v = v, u
		}
		p := pair{u, v}
		if _, exists := sampleNodes[p]; !exists {
			sampleNodes[p] = struct{}{}
		}
	}

	res := make([][]Node, 0, len(sampleNodes))
	for k := range sampleNodes {
		res = append(res, []Node{k.u, k.v})
	}

	return res
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
			_, err := algo.BFS(
				[]Node{p[0]},
				g.Adj,
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

	i := percentile*float64(sampleN+1) - 1
	floor_i := int(math.Floor(i))
	ans := float64(dists[floor_i]) + (i-float64(floor_i))*float64(dists[floor_i+1]-dists[floor_i])

	return ans, nil
}

func (g *Graph) TrianglesNumber() (int64, error) {
	if g.Directed {
		return 0, fmt.Errorf("graph must be undirected")
	}
	var triangles int64
	var mu sync.Mutex

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
				neighbors := g.Adj[node1]
				for node2 := range neighbors {
					if node2 > node1 {
						for node3 := range g.Adj[node2] {
							if _, ok := neighbors[node3]; ok && node3 > node2 {
								local++
							}
						}
					}
				}
			}
			mu.Lock()
			triangles += local
			mu.Unlock()
			return nil
		})
	}

	wp.Wait()

	return triangles, nil
}

func generateSnowballSample(
	component []Node,
	edges map[Node]map[Node]struct{},
) []Node {
	startNode := component[rand.IntN(len(component))]
	neighbors := make([]Node, 0, len(edges[startNode]))
	for k := range edges[startNode] {
		neighbors = append(neighbors, k)
	}

	nodes := make([]Node, 0, 3)
	nodes = append(nodes, startNode)
	nodes = append(nodes, neighbors[rand.IntN(len(neighbors))])
	if len(neighbors) > 1 {
		newNode := neighbors[rand.IntN(len(neighbors))]
		for newNode == nodes[1] {
			newNode = neighbors[rand.IntN(len(neighbors))]
		}
		nodes = append(nodes, newNode)
	}
	return nodes
}

func GetSnowballGraph(
	component []Node,
	edges map[Node]map[Node]struct{},
	maxNodeN int,
) (*Graph, error) {
	snowballSample := generateSnowballSample(component, edges)

	subgraph := NewGraph(false)

	subgraph.AddNode(snowballSample[0])
	stopCond := func(node Node, length int) bool {
		return subgraph.NumberOfNodes() >= min(maxNodeN, len(component))
	}

	onVisit := func(node, parent Node, length int) {
		subgraph.AddNode(node)
		subgraph.AddEdge(node, parent)
	}

	_, err := algo.BFS(snowballSample, edges, nil, onVisit, stopCond)
	if err != nil {
		return nil, err
	}

	return subgraph, nil
}

func FromFile(filePath string, directed bool) (*Graph, error) {
	file, err := os.Open(filePath)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	graph := NewGraph(directed)
	scanner := bufio.NewScanner(file)

	for scanner.Scan() {
		line := scanner.Text()
		if strings.HasPrefix(line, "#") || line == "" {
			continue
		}
		parts := strings.Fields(line)
		if len(parts) < 2 {
			continue
		}
		u, err1 := strconv.Atoi(parts[0])
		v, err2 := strconv.Atoi(parts[1])
		if err1 != nil || err2 != nil {
			continue
		}
		graph.AddEdge(Node(u), Node(v))
	}
	if err := scanner.Err(); err != nil {
		return nil, err
	}
	return graph, nil
}

func SortComponents(components [][]Node, desc bool) [][]Node {
	sort.Slice(components, func(i, j int) bool {
		if desc {
			return len(components[i]) > len(components[j])
		}
		return len(components[i]) < len(components[j])
	})
	return components
}
