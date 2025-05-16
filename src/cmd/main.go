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

	randomNode := wcc[0][rand.IntN(len(wcc[0])-1)]
	diameter := ugraph.GetDiameterDoubleSweep(randomNode)
	writef("Диаметр максимальной WCC, вычисленный методом The Double Sweep: %d\n", diameter)

	percentile90, err := graph.GetDistancePercentile(
		wcc[0],
		ugraph.Adj,
		0.9,
		500,
	)
	if err != nil {
		_ = fmt.Errorf("raised error: %v", err)
	}
	writef("90 процентиль расстояния между вершинами графа: %.2f\n", percentile90)

	snowballGraph, err := graph.GetSnowballGraph(wcc[0], ugraph.Adj, 500)
	if err != nil {
		_ = fmt.Errorf("raised error: %v", err)
	}
	wccSnowball, err := snowballGraph.FindWCC()
	if err != nil {
		_ = fmt.Errorf("raised error: %v", err)
	}
	randomNode = wccSnowball[0][rand.IntN(len(wccSnowball[0])-1)]
	diameter = snowballGraph.GetDiameterDoubleSweep(randomNode)
	percentile90, err = graph.GetDistancePercentile(wccSnowball[0], snowballGraph.Adj, 0.9, 500)
	if err != nil {
		_ = fmt.Errorf("raised error: %v", err)
	}
	writef("Диаметр максимальной WCC, вычисленный методом Snowball и The Double Sweep: %d\n", diameter)
	writef("90 процентиль расстояния между вершинами графа, вычисленный методом Snowball: %.2f\n", percentile90)
}

func getFileNameWithoutExt(path string) string {
	base := filepath.Base(path)
	ext := filepath.Ext(base)
	return base[:len(base)-len(ext)]
}
