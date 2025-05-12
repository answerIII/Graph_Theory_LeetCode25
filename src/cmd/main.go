package main

import (
	"fmt"
	"graph_theory/graph"
	"log"
	"math/rand/v2"
	"os"
	"path/filepath"
)

func main() {
	if len(os.Args) != 3 {
		fmt.Println("Usage: go run main.go <graph_file> <output_file>")
		os.Exit(1)
	}

	filePath := os.Args[1]
	outputPath := os.Args[2]
	graphName := getFileNameWithoutExt(filePath)

	g, err := graph.FromFile(filePath, true)
	if err != nil {
		log.Fatalf("Error reading graph file: %v\n", err)
	}

	ugraph := g.CastToUndirected()

	wcc, err := ugraph.FindWCC()
	if err != nil {
		log.Fatalf("Error finding WCC: %v\n", err)
	}
	scc, err := g.FindSCC()
	if err != nil {
		log.Fatalf("Error finding SCC: %v\n", err)
	}
	wcc = graph.SortComponents(wcc, true)
	scc = graph.SortComponents(scc, true)

	file, err := os.Create(outputPath)
	if err != nil {
		log.Fatalf("Error creating output file: %v\n", err)
	}
	defer file.Close()

	writef := func(format string, args ...interface{}) {
		_, _ = fmt.Fprintf(file, format, args...)
	}

	writef("Сводная информация о графе %s:\n\n", graphName)
	writef("Количество вершин: %d\n", ugraph.NumberOfNodes())
	writef("Количество рёбер: %d\n", ugraph.NumberOfEdges())
	writef("Плотность графа: %.6f\n", ugraph.Density())
	writef("Количество WCC: %d\n", len(wcc))
	writef("Доля вершин в максимальной WCC: %.6f\n", float64(len(wcc[0]))/float64(g.NumberOfNodes()))
	writef("Количество SCC: %d\n", len(scc))
	writef("Доля вершин в максимальной SCC: %.6f\n", float64(len(scc[0]))/float64(g.NumberOfNodes()))

	randomNode := wcc[0][rand.IntN(len(wcc)-1)]
	diameter := ugraph.GetDiameterDoubleSweep(randomNode)
	writef("Диаметр максимальной WCC, вычисленный методом The Double Sweep: %d\n", diameter)
}

func getFileNameWithoutExt(path string) string {
	base := filepath.Base(path)
	ext := filepath.Ext(base)
	return base[:len(base)-len(ext)]
}
