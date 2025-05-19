package main

import (
	"fmt"
	"graph_theory/graph"
	"graph_theory/tools"
	"log"
	"math/rand/v2"
	"os"
	"path/filepath"
)

const (
	SnowballSize         int     = 500 // Number of nodes to include in snowball graph
	Percentile           float64 = 0.9 // 90th percentile for distance calculation
	PercentileSampleSize int     = 500 // Number of random node pairs to sample when estimating the percentile
)

func main() {
	if len(os.Args) != 3 {
		fmt.Println("Usage: go run main.go <graph_file> <output_file>")
		os.Exit(1)
	}

	filePath := os.Args[1]
	outputPath := os.Args[2]
	graphName := getFileNameWithoutExt(filePath)
	sortedGraph := getFileDestinationWithoutExt(filePath) + "-sorted.txt"
	invertedGraph := getFileDestinationWithoutExt(filePath) + "-inverted.txt"
	undirectedGraph := getFileDestinationWithoutExt(filePath) + "-undirected.txt"

	log.Printf("Создание файла: %s\n", outputPath)
	file, err := os.Create(outputPath)
	if err != nil {
		log.Fatalf("Error creating output file: %v\n", err)
	}
	defer file.Close()

	writef := func(format string, args ...interface{}) {
		_, _ = fmt.Fprintf(file, format, args...)
	}

	log.Println("Создание вспомогательных файлов")
	tools.SortNodesInFile(filePath, sortedGraph)
	tools.InvertEdgesInFile(filePath, invertedGraph)
	tools.UndirectEdgesInFile(filePath, undirectedGraph)

	log.Println("Чтение и создание графа из файла")
	sccCount, maxSccSize := loadAndPrepareGraph(sortedGraph, invertedGraph)
	ugraph, err := graph.FromFile(undirectedGraph, false)

	log.Println("Поиск компонент слабой связности в неорграфе")
	wcc := getWCC(ugraph)
	randomNode := getRandomNode(wcc[0])
	log.Println("Расчет диаметра методом Double Sweep")
	maxWCCDiameterTDS := ugraph.GetDiameterDoubleSweep(randomNode)

	log.Printf("Расчет %.2f процентиля\n", Percentile*100)
	percentile := getPercentile(ugraph, wcc[0])
	log.Println("Генерация подграфа методом Snowball")
	snowball := getSnowball(ugraph, wcc[0])
	log.Println("Расчет диаметра методом Double Sweep на snowball подграфе")
	maxWCCDiameterSTDS := ugraph.GetDiameterDoubleSweep(getRandomNode(snowball))
	log.Printf("Расчет %.2f процентиля на snowball подграфе\n", Percentile*100)
	snowballPercentile := getPercentile(ugraph, snowball)

	log.Println("Подсчет треугольников в неорграфе")
	triangles := getTriangles(ugraph)
	log.Println("Нахождение среднего коэффициента кластеризации на неорграфе")
	avgCC := getAvgCC(ugraph, nil)
	log.Println("Нахождение глобального коэффициента кластеризации на неорграфе")
	globalCC := getGlobalCC(ugraph, triangles)
	log.Println("Нахождение среднего коэффициента кластеризации на наибольшей компоненте слабой связности")
	avgCcWcc := getAvgCC(ugraph, wcc[0])

	log.Println("Расчет степеней вершин неорграфа")
	minD, avgD, maxD := getDegrees(ugraph)

	// Output summary
	log.Println("Запись сводной информации о графе в файл")
	writef("Сводная информация о графе %s:\n\n", graphName)
	writef("Количество вершин: %d\n", ugraph.NumberOfNodes())
	writef("Количество рёбер: %d\n", ugraph.NumberOfEdges())
	writef("Плотность графа: %.6f\n", ugraph.Density())
	writef("Количество WCC: %d\n", len(wcc))
	writef("Доля вершин в наибольшей WCC: %.6f\n", float64(len(wcc[0]))/float64(ugraph.NumberOfNodes()))
	writef("Количество SCC: %d\n", sccCount)
	writef("Доля вершин в наибольшей SCC: %.6f\n", float64(maxSccSize)/float64(ugraph.NumberOfNodes()))
	writef("Диаметр наибольшей WCC (The Double Sweep): %d\n", maxWCCDiameterTDS)
	writef("%d процентиль расстояний: %.2f\n", int(Percentile*100), percentile)
	writef("Диаметр наибольшей WCC (Snowball + Double Sweep): %d\n", maxWCCDiameterSTDS)
	writef("%d процентиль расстояний (Snowball): %.2f\n", int(Percentile*100), snowballPercentile)
	writef("Количество треугольников: %d\n", triangles)
	writef("Средний коэффициент кластеризации: %.4f\n", avgCC)
	writef("Глобальный коэффициент кластеризации: %.4f\n", globalCC)
	writef("Средний коэффициент кластеризации (largest WCC): %.4f\n", avgCcWcc)
	writef("Минимальная степень узлов: %d\n", minD)
	writef("Средняя степень узлов: %.2f\n", avgD)
	writef("Максимальная степень узлов: %d\n", maxD)
}

// Utility

func getFileNameWithoutExt(path string) string {
	base := filepath.Base(path)
	return base[:len(base)-len(filepath.Ext(base))]
}

func getFileDestinationWithoutExt(path string) string {
	return path[:len(path)-len(filepath.Ext(path))]
}

// Graph processing helpers

func loadAndPrepareGraph(sortedGraph, invertedGraph string) (int, int) {
	g, err := graph.FromFile(sortedGraph, true)
	if err != nil {
		log.Fatalf("Error loading graph: %v", err)
	}

	log.Println("Нахождение компонент сильной связности")
	scc, err := g.FindSCC(invertedGraph)
	if err != nil {
		log.Fatalf("Error finding SCC: %v", err)
	}

	log.Println("Сортировка компонент сильной связности")
	scc = graph.SortComponents(scc, true)
	return len(scc), len(scc[0])
}

func getWCC(g *graph.Graph) [][]graph.Node {
	wcc, err := g.FindWCC()
	if err != nil {
		log.Fatalf("Error finding WCC: %v", err)
	}
	return graph.SortComponents(wcc, true)
}

func getRandomNode(nodes []graph.Node) graph.Node {
	return nodes[rand.IntN(len(nodes))]
}

func getPercentile(g *graph.Graph, nodes []graph.Node) float64 {
	val, err := g.GetDistancePercentile(nodes, Percentile, PercentileSampleSize)
	if err != nil {
		log.Printf("Error calculating percentile: %v", err)
	}
	return val
}

func getSnowball(g *graph.Graph, base []graph.Node) []graph.Node {
	snowballGraph, err := graph.GetSnowballGraph(g, base, SnowballSize)
	if err != nil {
		log.Printf("Error building snowball: %v", err)
	}
	wcc, err := snowballGraph.FindWCC()
	if err != nil {
		log.Printf("Error finding WCC in snowball: %v", err)
	}
	return graph.SortComponents(wcc, true)[0]
}

func getTriangles(g *graph.Graph) int64 {
	val, err := g.TrianglesNumber()
	if err != nil {
		log.Printf("Error counting triangles: %v", err)
	}
	return val
}

func getAvgCC(g *graph.Graph, nodes []graph.Node) float64 {
	val, err := g.GetAverageClusteringCoefficient(nodes)
	if err != nil {
		log.Printf("Error calculating average clustering coefficient: %v", err)
	}
	return val
}

func getGlobalCC(g *graph.Graph, triangles int64) float64 {
	val, err := g.GetGlobalClusteringCoefficient(triangles)
	if err != nil {
		log.Printf("Error calculating global clustering coefficient: %v", err)
	}
	return val
}

func getDegrees(g *graph.Graph) (int, float64, int) {
	minD, avgD, maxD, err := g.ProcessNodesDegrees("")
	if err != nil {
		log.Printf("Error processing degrees: %v", err)
	}
	return minD, avgD, maxD
}
