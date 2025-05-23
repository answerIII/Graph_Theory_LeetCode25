package main

import (
	"fmt"
	"graph_theory/tools"
	"log"
	"os"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Println("Usage: go run generate.go <graph_file>")
		os.Exit(1)
	}
	filePath := os.Args[1]

	graphName := tools.GetFileNameWithoutExt(filePath)
	auxPath := tools.GetFileDestination(filePath) + "aux_graphs/"
	if err := os.MkdirAll(auxPath, 0o775); err != nil {
		log.Fatalf("Error creating directory: %v\n", err)
	}
	auxSortedGraph := auxPath + graphName + "-sorted.txt"
	auxInvertedGraph := auxPath + graphName + "-inverted.txt"
	auxUndirectedGraph := auxPath + graphName + "-undirected.txt"

	log.Println("Создание вспомогательных файлов: " + graphName)
	if err := tools.SortNodesInFile(filePath, auxSortedGraph); err != nil {
		log.Fatalf("Can't create file %v\n", auxSortedGraph)
	}
	if err := tools.InvertEdgesInFile(filePath, auxInvertedGraph); err != nil {
		log.Fatalf("Can't create file %v\n", auxInvertedGraph)
	}
	if err := tools.UndirectEdgesInFile(filePath, auxUndirectedGraph); err != nil {
		log.Fatalf("Can't create file %v\n", auxUndirectedGraph)
	}
}
