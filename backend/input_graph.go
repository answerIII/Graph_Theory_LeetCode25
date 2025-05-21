package main

import (
	"encoding/csv"
	"encoding/json"
	"io"
	"os"
	"strconv"

	"github.com/HikkMind/graph/structs"
	"github.com/vmihailenco/msgpack/v5"
)

func readGraph(filename string) structs.Graph {
	length := len(filename)
	if length > 4 && filename[length-5:] == ".json" {
		return readGraphJSON(filename)
	} else if length > 3 && filename[length-4:] == ".csv" {
		return readGraphCSV(filename)
	} else if length > 7 && filename[length-8:] == ".msgpack" {
		return readGraphMP(filename)
	}
	return structs.Graph{}
}

func readGraphCSV(filename string) structs.Graph {

	file, err := os.Open(filename)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	outputGraph := structs.Graph{AdjList: make(map[int][]int)}
	if outputGraph.Directed {
		outputGraph.Edges = make([]structs.Edge, 0)
	}

	reader := csv.NewReader(file)

	for range 4 {
		record, err := reader.Read()
		if err == io.EOF {
			return structs.Graph{}
		}

		if record[0] == "directed" {
			if record[1] == "true" {
				outputGraph.Directed = true
			} else {
				outputGraph.Directed = false
			}
		} else if record[0] == "numNodes" {
			intNumNodes, err := strconv.Atoi(record[1])
			if err != nil {
				panic(err)
			}
			outputGraph.VertexCount = intNumNodes
		} else if record[0] == "numEdges" {
			intNumEdges, err := strconv.Atoi(record[1])
			if err != nil {
				panic(err)
			}
			outputGraph.EdgesCount = intNumEdges
		} else if record[0] != "from" && record[1] != "to" {
			panic("unknown csv file format")
		}

	}

	for {
		record, err := reader.Read()
		if err == io.EOF {
			break
		}

		u, err := strconv.Atoi(record[0])
		if err != nil {
			panic(err)
		}
		v, err := strconv.Atoi(record[1])
		if err != nil {
			panic(err)
		}

		if adj, ok := outputGraph.AdjList[u]; ok {
			outputGraph.AdjList[u] = append(adj, v)
		} else {
			outputGraph.AdjList[u] = make([]int, 1)
			outputGraph.AdjList[u][0] = v
		}

		if outputGraph.Directed {
			outputGraph.Edges = append(outputGraph.Edges, structs.Edge{From: u, To: v})
			continue
		}

		if adj, ok := outputGraph.AdjList[v]; ok {
			outputGraph.AdjList[v] = append(adj, u)
		} else {
			outputGraph.AdjList[v] = make([]int, 1)
			outputGraph.AdjList[v][0] = u
		}

	}

	return outputGraph
}

func readGraphJSON(filename string) structs.Graph {
	file, err := os.Open(filename)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	var graphJson structs.GraphJSON
	err = json.NewDecoder(file).Decode(&graphJson)
	if err != nil {
		panic(err)
	}

	outputGraph := structs.Graph{Directed: graphJson.Directed, VertexCount: graphJson.NumNodes, EdgesCount: graphJson.NumEdges, AdjList: make(map[int][]int)}
	if outputGraph.Directed {
		outputGraph.Edges = graphJson.Edges
	}

	for _, edge := range graphJson.Edges {
		u, v := edge.From, edge.To

		if adj, ok := outputGraph.AdjList[u]; ok {
			outputGraph.AdjList[u] = append(adj, v)
		} else {
			outputGraph.AdjList[u] = make([]int, 1)
			outputGraph.AdjList[u][0] = v
		}

		if outputGraph.Directed {
			continue
		}

		if adj, ok := outputGraph.AdjList[v]; ok {
			outputGraph.AdjList[v] = append(adj, u)
		} else {
			outputGraph.AdjList[v] = make([]int, 1)
			outputGraph.AdjList[v][0] = u
		}

	}

	return outputGraph
}

func readGraphMP(filename string) structs.Graph {

	dataMsg, err := os.ReadFile(filename)
	if err != nil {
		panic(err)
	}

	graphMsg := structs.GraphMsg{Edges: make([][]int, 0)}
	err = msgpack.Unmarshal(dataMsg, &graphMsg)
	if err != nil {
		panic(err)
	}

	outputGraph := structs.Graph{
		Directed:    graphMsg.Directed,
		VertexCount: graphMsg.VertexCount,
		EdgesCount:  len(graphMsg.Edges),
		AdjList:     make(map[int][]int),
	}
	// if outputGraph.Directed {
	// 	outputGraph.Edges = graphMsg.Edges
	// }
	for _, edge := range graphMsg.Edges {
		//fmt.Println(edge)
		u, v := edge[0], edge[1]
		// u, v := edge.From, edge.To
		// u, _ := strconv.Atoi(edge[0])
		// v, _ := strconv.Atoi(edge[1])
		if outputGraph.Directed {
			outputGraph.Edges = append(outputGraph.Edges, structs.Edge{From: u, To: v})
		}

		if adj, ok := outputGraph.AdjList[u]; ok {
			outputGraph.AdjList[u] = append(adj, v)
		} else {
			outputGraph.AdjList[u] = make([]int, 1)
			outputGraph.AdjList[u][0] = v
		}

		if outputGraph.Directed {
			continue
		}

		if adj, ok := outputGraph.AdjList[v]; ok {
			outputGraph.AdjList[v] = append(adj, u)
		} else {
			outputGraph.AdjList[v] = make([]int, 1)
			outputGraph.AdjList[v][0] = u
		}

	}

	return outputGraph
}
