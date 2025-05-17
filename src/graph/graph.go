package graph

import (
	"bufio"
	"fmt"
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

func (g *Graph) getNodesSlice() []Node {
	nodes := make([]Node, 0, g.NumberOfNodes())
	for node, _ := range g.Nodes {
		nodes = append(nodes, node)
	}
	sort.Slice(nodes, func(i, j int) bool {
		return nodes[i] < nodes[j]
	})
	return nodes
}

func (g *Graph) GetNeighborsRandomSlice(node Node) []Node {
	neighbors := make([]Node, 0, len(g.Adj[node]))
	for u := range g.Adj[node] {
		neighbors = append(neighbors, u)
	}
	return neighbors
}

func (g *Graph) GetNeighborsMap(node Node) map[Node]struct{} {
	return g.Adj[node]
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
	for u := range g.Nodes {
		neighbors := g.GetNeighborsMap(u)
		for v := range neighbors {
			inverted.AddEdge(v, u)
		}
	}
	return inverted
}

func (g *Graph) CastToUndirected() *Graph {
	if !g.Directed {
		panic("Graph is already undirected")
	}
	ug := NewGraph(false)

	for u := range g.Nodes {
		neighbors := g.GetNeighborsMap(u)
		for v := range neighbors {
			ug.AddEdge(u, v)
			ug.AddEdge(v, u)
		}
	}

	return ug
}

func GetSnowballGraph(
	baseGraph *Graph,
	component []Node,
	maxNodeN int,
) (*Graph, error) {
	snowballSample := generateSnowballSample(baseGraph, component)

	subgraph := NewGraph(false)

	subgraph.AddNode(snowballSample[0])
	stopCond := func(node Node, length int) bool {
		return subgraph.NumberOfNodes() >= min(maxNodeN, len(component))
	}

	onVisit := func(node, parent Node, length int) {
		subgraph.AddNode(node)
		subgraph.AddEdge(node, parent)
	}

	_, err := BFS(baseGraph, snowballSample, nil, onVisit, stopCond)
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
