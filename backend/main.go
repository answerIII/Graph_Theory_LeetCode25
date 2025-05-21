package main

import (
	"fmt"
	"os"
)

func main() {

	stdFileName := "test.json"

	if len(os.Args) > 1 {
		stdFileName = os.Args[1]
	}

	graph := readGraph(stdFileName)
	fmt.Println("got graph")
	fmt.Println(graph.VertexCount, graph.EdgesCount)

}
