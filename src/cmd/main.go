package main

import (
	"errors"
	"fmt"
	"graph_theory/graph"
	"graph_theory/tools"
	"graph_theory/workerpool"
	"log"
	"math/rand/v2"
	"os"
	"sort"
)

var SnowballSizes []int = []int{500, 1000}         // Number of nodes to include in snowball graph
var PercentileSampleSizes []int = []int{500, 1000} // Number of random node pairs to sample when estimating the percentile

const (
	Percentile        float64 = 0.9 // 90th percentile for distance calculation
	EstimateTestCount int     = 50  // Number of tests when estimating
)

func main() {
	if len(os.Args) != 3 {
		fmt.Println("Usage: go run main.go <graph_file> <output_file>")
		os.Exit(1)
	}

	filePath := os.Args[1]
	outputPath := os.Args[2]
	graphName := tools.GetFileNameWithoutExt(filePath)
	auxPath := tools.GetFileDestination(filePath) + "aux_graphs/"
	if err := os.MkdirAll(auxPath, 0o775); err != nil {
		log.Fatalf("Error creating directory: %v\n", err)
	}
	auxSortedGraph := auxPath + graphName + "-sorted.txt"
	auxInvertedGraph := auxPath + graphName + "-inverted.txt"
	auxUndirectedGraph := auxPath + graphName + "-undirected.txt"

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
	if _, err := os.Stat(auxSortedGraph); errors.Is(err, os.ErrNotExist) {
		err := tools.SortNodesInFile(filePath, auxSortedGraph)
		if err != nil {
			fmt.Printf("Error in SortNodesInFile(): %v\n", err)
			return
		}
	}
	if _, err := os.Stat(auxInvertedGraph); errors.Is(err, os.ErrNotExist) {
		err := tools.InvertEdgesInFile(filePath, auxInvertedGraph)
		if err != nil {
			fmt.Printf("Error in InvertEdgesInFile(): %v\n", err)
			return
		}
	}
	if _, err := os.Stat(auxUndirectedGraph); errors.Is(err, os.ErrNotExist) {
		err := tools.UndirectEdgesInFile(filePath, auxUndirectedGraph)
		if err != nil {
			fmt.Printf("Error in UndirectEdgesInFile(): %v\n", err)
			return
		}
	}

	log.Println("Чтение и создание графа из файла")
	sccCount, maxSccSize := loadAndPrepareGraph(auxSortedGraph, auxInvertedGraph)
	ugraph, err := graph.FromFile(auxUndirectedGraph, false)
	if err != nil {
		fmt.Printf("Error reading graph from file: %v", err)
		return
	}

	log.Println("Поиск компонент слабой связности в неорграфе")
	wcc := getWCC(ugraph)

	maxWCCDiameterTDS := 0
	percentiles := make([]float64, len(PercentileSampleSizes))
	maxWCCDiameterSTDS := make([]int, len(SnowballSizes))
	snowballPercentiles := make([]float64, len(SnowballSizes))

	for i := range EstimateTestCount {
		log.Printf("Iteration %d / %d\n", i, EstimateTestCount)

		randomNode := getRandomNode(wcc[0])
		log.Println("Расчет диаметра методом Double Sweep")
		maxWCCDiameterTDS += ugraph.GetDiameterDoubleSweep(randomNode)

		log.Printf("Расчет %.2f процентиля\n", Percentile*100)
		for j, PercentileSampleSize := range PercentileSampleSizes {
			percentiles[j] += getPercentile(ugraph, wcc[0], PercentileSampleSize)
		}

		log.Println("Генерация подграфа методом Snowball")
		log.Println("Расчет диаметра методом Double Sweep на snowball подграфе")
		log.Printf("Расчет %.2f процентиля на snowball подграфе\n", Percentile*100)
		for j, SnowballSize := range SnowballSizes {
			snowball := getSnowball(ugraph, wcc[0], SnowballSize)
			snowballNodes := snowball.GetNodesSlice()
			maxWCCDiameterSTDS[j] += snowball.GetDiameterDoubleSweep(getRandomNode(snowballNodes))
			snowballPercentiles[j] += getPercentile(snowball, snowballNodes, SnowballSize)
		}
	}

	log.Println("Подсчет треугольников в неорграфе")
	triangles := getTriangles(ugraph)
	log.Println("Нахождение среднего коэффициента кластеризации на неорграфе")
	var avgCC, avgCcWcc float64
	avgCC = getAvgCC(ugraph, nil)
	log.Println("Нахождение глобального коэффициента кластеризации на неорграфе")
	globalCC := getGlobalCC(ugraph, triangles)
	log.Println("Нахождение среднего коэффициента кластеризации на наибольшей компоненте слабой связности")
	if len(wcc) == 1 {
		avgCcWcc = avgCC
	} else {
		avgCcWcc = getAvgCC(ugraph, wcc[0])
	}

	log.Println("Расчет степеней вершин неорграфа")
	minD, avgD, maxD := getDegrees(ugraph, "")

	log.Println("Удаление x% случайных вершин и вершин максимальной степени")
	percents := []float64{0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.6, 0.7, 0.8, 0.9}
	wccRatioRandomNodes, wccRatioMaxDegree := computeWCCRatiosAfterRemovals(ugraph, percents, getWCC, 1)

	radius, diameter := ugraph.GetPreciseRadiusAndDiameter()

	teamNode, _ := graph.GetMappedNode(tools.GetFileDestination(filePath)+"mapper/"+graphName+"-mapper.txt", 5)
	teamNodeClustering, _ := ugraph.GetLocalClusteringCoefficient(teamNode)

	// Output summary
	log.Println("Запись сводной информации о графе в файл")
	writef("Сводная информация о графе %s:\n\n", graphName)
	writef("Количество вершин: %d\n", ugraph.NumberOfNodes())
	writef("Количество рёбер: %d\n", ugraph.EdgesNumber)
	writef("Плотность графа: %.6f\n", ugraph.Density())
	writef("Количество WCC: %d\n", len(wcc))
	writef("Доля вершин в наибольшей WCC: %.6f\n", float64(len(wcc[0]))/float64(ugraph.NumberOfNodes()))
	writef("Количество SCC: %d\n", sccCount)
	writef("Доля вершин в наибольшей SCC: %.6f\n", float64(maxSccSize)/float64(ugraph.NumberOfNodes()))
	writef("Точный диаметр графа: %d\n", diameter)
	writef("Точный радиус графа: %d\n", radius)
	writef("Диаметр наибольшей WCC (The Double Sweep): %.2f\n", float64(maxWCCDiameterTDS)/float64(EstimateTestCount))
	for i, PercentileSampleSize := range PercentileSampleSizes {
		writef("%d процентиль расстояний (random %d): %.2f\n", int(Percentile*100), PercentileSampleSize, percentiles[i]/float64(EstimateTestCount))
	}
	for i, SnowballSize := range SnowballSizes {

		writef("Диаметр наибольшей WCC (Snowball-%d + Double Sweep): %.2f\n", SnowballSize, float64(maxWCCDiameterSTDS[i])/float64(EstimateTestCount))
	}
	for i, SnowballSize := range SnowballSizes {

		writef("%d процентиль расстояний (Snowball-%d): %.2f\n", int(Percentile*100), SnowballSize, snowballPercentiles[i]/float64(EstimateTestCount))
	}
	writef("Количество треугольников: %d\n", triangles)
	writef("Кластерный коэффициент вершины номера команды (%d): %.2f\n", 5, teamNodeClustering)
	writef("Средний коэффициент кластеризации: %.4f\n", avgCC)
	writef("Глобальный коэффициент кластеризации: %.4f\n", globalCC)
	writef("Средний коэффициент кластеризации (largest WCC): %.4f\n", avgCcWcc)
	writef("Минимальная степень узлов: %d\n", minD)
	writef("Средняя степень узлов: %.2f\n", avgD)
	writef("Максимальная степень узлов: %d\n", maxD)
	writef("\nУдаление x%% узлов и влияние на наибольшую компоненту слабой связности:\n\n")
	writef("\tУдаление случайных узлов:\n")
	for i, percent := range percents {
		writef("\t\tx = %.0f%%: доля вершин в наибольшей WCC: %.4f\n", percent*100, wccRatioRandomNodes[i])
	}
	writef("\n\tУдаление узлов наибольшей степени:\n")
	for i, percent := range percents {
		writef("\t\tx = %.0f%%: доля вершин в наибольшей WCC: %.4f\n", percent*100, wccRatioMaxDegree[i])
	}
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

func getPercentile(g *graph.Graph, nodes []graph.Node, PercentileSampleSize int) float64 {
	val, err := g.GetDistancePercentile(nodes, Percentile, PercentileSampleSize)
	if err != nil {
		log.Printf("Error calculating percentile: %v", err)
	}
	return val
}

func getSnowball(g *graph.Graph, base []graph.Node, SnowballSize int) *graph.Graph {
	snowballGraph, err := graph.GetSnowballGraph(g, base, SnowballSize)
	if err != nil {
		log.Printf("Error building snowball: %v", err)
	}
	return snowballGraph
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

func getDegrees(g *graph.Graph, filepath string) (int, float64, int) {
	minD, avgD, maxD, err := g.ProcessNodesDegrees(filepath)
	if err != nil {
		log.Printf("Error processing degrees: %v", err)
	}
	return minD, avgD, maxD
}

func computeWCCRatiosAfterRemovals(
	ugraph *graph.Graph,
	percents []float64,
	getWCC func(*graph.Graph) [][]graph.Node,
	workersNumber int,
) ([]float64, []float64) {
	type removalResult struct {
		percent  float64
		wccRatio float64
		fromMax  bool // true — max degree, false — random
	}

	wp := workerpool.NewWorkerPool(workersNumber, len(percents)*2)
	defer wp.Shutdown()
	resultsCh := make(chan removalResult, len(percents)*2)

	for _, percent := range percents {
		p := percent

		// Task for random node removal
		wp.Submit(func() error {
			log.Printf("Удаления %.0f%% случайных вершин\n", p*100)
			ugraphCopy := ugraph.DeepCopy()
			ugraphCopy.RemoveRandomNodes(p)
			wcc := getWCC(ugraphCopy)
			ratio := float64(len(wcc[0])) / float64(ugraphCopy.NumberOfNodes())
			resultsCh <- removalResult{percent: p, wccRatio: ratio, fromMax: false}
			return nil
		})

		// Task for max-degree node removal
		wp.Submit(func() error {
			log.Printf("Удаление %.0f%% вершин максимальной степени\n", p*100)
			ugraphCopy := ugraph.DeepCopy()
			ugraphCopy.RemoveHighestDegreeNodes(p)
			wcc := getWCC(ugraphCopy)
			ratio := float64(len(wcc[0])) / float64(ugraphCopy.NumberOfNodes())
			resultsCh <- removalResult{percent: p, wccRatio: ratio, fromMax: true}
			return nil
		})
	}

	wp.Wait()
	close(resultsCh)

	var (
		randomResults []removalResult
		maxDegResults []removalResult
	)

	for v := range resultsCh {
		if v.fromMax {
			maxDegResults = append(maxDegResults, v)
		} else {
			randomResults = append(randomResults, v)
		}
	}

	sort.Slice(randomResults, func(i, j int) bool {
		return randomResults[i].percent < randomResults[j].percent
	})
	sort.Slice(maxDegResults, func(i, j int) bool {
		return maxDegResults[i].percent < maxDegResults[j].percent
	})

	wccRatioRandomNodes := make([]float64, len(percents))
	wccRatioMaxDegree := make([]float64, len(percents))
	for i := range percents {
		wccRatioRandomNodes[i] = randomResults[i].wccRatio
		wccRatioMaxDegree[i] = maxDegResults[i].wccRatio
	}

	return wccRatioRandomNodes, wccRatioMaxDegree
}
