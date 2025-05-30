package graph

import (
	"bufio"
	"math"
	"math/rand/v2"
	"os"
	"slices"
	"sort"
	"strconv"
	"strings"
)

type Node int32
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

func NewGraph(directed bool, numberRealEdges, numberNodes, numberEdges int) *Graph {
	return &Graph{
		Directed: directed,
		Adj: Adjacency{
			to:  make([]Node, 0, numberRealEdges),
			ptr: make([]int, 1, numberNodes+1)},
		Nodes:       make(Nodes),
		EdgesNumber: numberRealEdges,
	}
}

func (g *Graph) GetNodesSlice() []Node {
	nodes := make([]Node, 0, g.NumberOfNodes())
	for node := range g.Nodes {
		nodes = append(nodes, node)
	}
	slices.Sort(nodes)
	return nodes
}

func (g *Graph) RemoveNodes(nodes map[Node]struct{}) {
	// update "ptr" slice
	updPtr := 0
	prevVal := 0
	for i := 1; i < len(g.Adj.ptr); i++ { // iter over g.Adj.ptr
		currNode := Node(i - 1)
		for j := prevVal; j < g.Adj.ptr[i]; j++ { // iter over [prevVal, g.Adj.ptr[i]) colIdx in g.Adj.to
			_, ok1 := nodes[g.Adj.to[j]]
			_, ok2 := nodes[currNode]
			if ok1 || ok2 {
				g.Adj.to[j] = -1 // mark deleted node
				updPtr++
				g.EdgesNumber--
			}
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
	for node := range nodes {
		delete(g.Nodes, node)
	}
}

func (g *Graph) RemoveRandomNodes(percent float64) {
	n := g.NumberOfNodes()
	numberOfRemovedNodes := int(math.Floor(float64(n) * percent))
	nodes := make(map[Node]struct{})
	for range numberOfRemovedNodes {
		var x Node
		for {
			x = Node(rand.IntN(n))
			if _, exists := nodes[x]; !exists {
				break
			}
		}
		nodes[x] = struct{}{}
	}
	g.RemoveNodes(nodes)
}

func (g *Graph) RemoveHighestDegreeNodes(percent float64) {
	n := g.NumberOfNodes()
	numberOfRemovedNodes := int(math.Floor(float64(n) * percent))
	nodes := make(map[Node]struct{})

	type nodeDegree struct {
		node   Node
		degree int
	}
	ndSlice := make([]nodeDegree, n)
	for i := 0; i < n; i++ {
		node := Node(i)
		ndSlice[i] = nodeDegree{
			node:   node,
			degree: g.DegreeNode(node),
		}
	}
	sort.Slice(ndSlice, func(i, j int) bool {
		return ndSlice[i].degree > ndSlice[j].degree
	})

	for i := range numberOfRemovedNodes {
		nodes[ndSlice[i].node] = struct{}{}
	}

	g.RemoveNodes(nodes)
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

func (g *Graph) DeepCopy() *Graph {
	newNodes := make(Nodes)
	for k := range g.Nodes {
		newNodes[k] = struct{}{}
	}
	newAdj := Adjacency{
		to:  make([]Node, len(g.Adj.to)),
		ptr: make([]int, len(g.Adj.ptr)),
	}
	copy(newAdj.to, g.Adj.to)
	copy(newAdj.ptr, g.Adj.ptr)

	return &Graph{
		Directed:    g.Directed,
		Adj:         newAdj,
		Nodes:       newNodes,
		EdgesNumber: g.EdgesNumber,
	}
}

func GetSnowballGraph(
	baseGraph *Graph,
	component []Node,
	maxNodeN int,
) (*Graph, error) {
	snowballSample := generateSnowballSample(baseGraph, component)

	mapper := map[Node]int{}
	var nodes []int
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

	adjLen := 0
	slices.Sort(nodes)
	for _, u := range nodes {
		slices.Sort(adj[u])
		adjLen += len(adj[u])
	}

	subgraph := NewGraph(false, adjLen, len(nodes), adjLen)

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

	scanner := bufio.NewScanner(file)

	scanner.Scan()
	parts := strings.Fields(scanner.Text())

	numberRealEdges, err := strconv.Atoi(parts[0])
	numberNodes, err := strconv.Atoi(parts[1])
	numberCompressedEdges, err := strconv.Atoi(parts[2])
	if err != nil {
		return nil, err
	}

	graph := NewGraph(directed, numberRealEdges, numberNodes, numberCompressedEdges)

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

func GetMappedNode(filePath string, node Node) (Node, error) {
	mapper := map[Node]Node{}

	file, err := os.Open(filePath)
	if err != nil {
		return -1, err
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)

	for scanner.Scan() {
		parts := strings.Fields(scanner.Text())

		u, err1 := strconv.Atoi(parts[0])
		v, err2 := strconv.Atoi(parts[1])
		if err1 != nil || err2 != nil {
			continue
		}
		mapper[Node(u)] = Node(v)
	}
	return mapper[Node(node)], nil
}
