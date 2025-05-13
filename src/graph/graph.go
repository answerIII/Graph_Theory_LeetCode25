package graph

import (
	"bufio"
	"errors"
	"fmt"
	algo "graph_theory/graph/algorithms"
	"log"
	"os"
	"sort"
	"strconv"
	"strings"
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

// TODO: number of SCC is not determined
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
	farnodeA, _, err := algo.BFS(source, g.Adj, nil)
	if err != nil {
		log.Fatalf("Error in BFS: %v\n", err)
	}

	source[0] = farnodeA
	_, diameter, err := algo.BFS(source, g.Adj, nil)
	if err != nil {
		log.Fatalf("Error in BFS: %v\n", err)
	}
	return diameter
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
