package graph

import (
	"bufio"
	"os"
	"slices"
	"strconv"
	"strings"
)

type Node int
type Nodes map[Node]struct{}
type Adjacency struct {
	// value struct{}
	to  []Node
	ptr []int
}

func (a *Adjacency) neighbors(u Node) []Node {
	return a.to[a.ptr[u]:a.ptr[u+1]]
}

type Graph struct {
	Directed    bool
	Adj         Adjacency
	Nodes       Nodes
	EdgesNumber int
}

func NewGraph(directed bool) *Graph {
	return &Graph{
		Directed: directed,
		Adj:      Adjacency{ptr: []int{0}},
		Nodes:    make(Nodes),
	}
}

func (g *Graph) getNodesSlice() []Node {
	nodes := make([]Node, 0, g.NumberOfNodes())
	for node := range g.Nodes {
		nodes = append(nodes, node)
	}
	slices.Sort(nodes)
	return nodes
}

func (g *Graph) RemoveNode(node Node) {
	// update "ptr" slice
	quantityEdgesInNodeRow := g.Adj.ptr[node+1] - g.Adj.ptr[node]

	updPtr := 0
	prevVal := 0
	for i := 1; i < len(g.Adj.ptr); i++ { // iter over g.Adj.ptr
		for j := prevVal; j < g.Adj.ptr[i]; j++ { // iter over [prevVal, g.Adj.ptr[i]) colIdx in g.Adj.to
			if g.Adj.to[j] == node {
				g.Adj.to[j] = -1 // mark deleted node
				updPtr++
			}
		}
		if Node(i-1) == node {
			updPtr += quantityEdgesInNodeRow
		}
		prevVal = g.Adj.ptr[i]
		g.Adj.ptr[i] -= updPtr
	}

	// remove -1 from "to" slice
	writePtr := 0
	for writePtr < len(g.Adj.to) && g.Adj.to[writePtr] != -1 {
		writePtr++
	}
	for i := writePtr; i < len(g.Adj.to); i++ {
		if g.Adj.to[i] != -1 {
			g.Adj.to[writePtr] = g.Adj.to[i]
			writePtr++
		}
	}
	g.Adj.to = g.Adj.to[:writePtr]
	delete(g.Nodes, node)
}

func (g *Graph) HasNode(n Node) bool {
	_, exists := g.Nodes[n]
	return exists
}

func (g *Graph) AddEdge(u, v Node) {
	if _, ok := g.Nodes[u]; !ok {
		g.Nodes[u] = struct{}{}
	}
	if _, ok := g.Nodes[v]; !ok {
		g.Nodes[v] = struct{}{}
	}
	if int(u) != len(g.Adj.ptr)-1 {
		nodesToInsert := int(u) - len(g.Adj.ptr) + 1
		for range nodesToInsert {
			g.Adj.ptr = append(g.Adj.ptr, len(g.Adj.to))
		}
	}
	g.Adj.to = append(g.Adj.to, v)
}

func (g *Graph) HasEdge(u, v Node) bool {
	_, exists := slices.BinarySearch(g.Adj.neighbors(u), v)
	return exists
}

func GetSnowballGraph(
	baseGraph *Graph,
	component []Node,
	maxNodeN int,
) (*Graph, error) {
	snowballSample := generateSnowballSample(baseGraph, component)

	mapper := map[Node]int{}
	nodes := []int{}
	adj := map[int][]int{}

	mapperIdx := 0

	stopCond := func(node Node, length int) bool {
		return len(nodes) >= min(maxNodeN, len(component))
	}

	onVisit := func(node, parent Node, length int) {
		if _, has := mapper[node]; !has {
			mapper[node] = mapperIdx
			mapperIdx++
			nodes = append(nodes, mapper[node])
		}
		if _, has := mapper[parent]; !has {
			mapper[parent] = mapperIdx
			mapperIdx++
			nodes = append(nodes, mapper[parent])
		}
		adj[mapper[node]] = append(adj[mapper[node]], mapper[parent])
		adj[mapper[parent]] = append(adj[mapper[parent]], mapper[node])
	}

	_, err := BFS(baseGraph, snowballSample, nil, onVisit, stopCond)
	if err != nil {
		return nil, err
	}
	slices.Sort(nodes)
	for _, u := range nodes {
		slices.Sort(adj[u])
	}

	subgraph := NewGraph(false)

	for _, u := range nodes {
		for _, v := range adj[u] {
			subgraph.AddEdge(Node(u), Node(v))
		}
	}

	nodesToInsert := len(nodes) - len(subgraph.Adj.ptr) + 1
	for range nodesToInsert {
		subgraph.Adj.ptr = append(subgraph.Adj.ptr, len(subgraph.Adj.to))
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
			graph.EdgesNumber, _ = strconv.Atoi(parts[0])
			continue
		}
		u, err1 := strconv.Atoi(parts[0])
		v, err2 := strconv.Atoi(parts[1])
		if err1 != nil || err2 != nil {
			continue
		}
		graph.AddEdge(Node(u), Node(v))
	}
	nodesToInsert := len(graph.Nodes) - len(graph.Adj.ptr) + 1
	for range nodesToInsert {
		graph.Adj.ptr = append(graph.Adj.ptr, len(graph.Adj.to))
	}
	if err := scanner.Err(); err != nil {
		return nil, err
	}
	return graph, nil
}
