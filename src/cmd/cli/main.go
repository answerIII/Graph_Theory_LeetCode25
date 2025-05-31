package main

import (
	"bufio"
	"errors"
	"fmt"
	"graph_theory/graph"
	"graph_theory/graph/landmarkAlgo"
	"graph_theory/tools"
	"math/rand/v2"
	"os"
	"strconv"
	"strings"
	"sync"
)

const (
	SnowballSize         int = 500 // Number of nodes to include in snowball graph
	PercentileSampleSize int = 500 // Number of random node pairs to sample when estimating the percentile
)

// paths
var (
	auxSortedGraph     string
	auxInvertedGraph   string
	auxUndirectedGraph string
)

func main() {
	var ug *graph.Graph
	var wcc [][]graph.Node
	var triangles int64 = -1
	var graphName string

	reader := bufio.NewReader(os.Stdin)
	fmt.Println("Graph CLI started. Type '0' for help.")

	for {
		fmt.Print("> ")
		input, err := reader.ReadString('\n')
		if err != nil {
			fmt.Printf("Error reading input: %v\n", err)
			continue
		}

		input = strings.TrimSpace(input)
		if input == "" {
			continue
		}

		args := strings.Split(input, " ")

		cmd := args[0]
		switch cmd {
		case "0": // help
			printHelp()
		case "1": // load graph
			if len(args) < 3 {
				fmt.Println("Usage: 1 <graph_type> <graph_name>")
				continue
			}

			ug = nil
			wcc = nil
			triangles = -1

			graphName = args[2]
			filePath := getFilepath(args[1], graphName)
			ug, err = loadGraph(filePath)
			if err != nil {
				fmt.Printf("Error loading graph: %v\n", err)
			} else {
				fmt.Println("Graph loaded successfully")
			}
		case "2": // number and size of SCC
			if ug == nil {
				fmt.Println("Load a graph first.")
				continue
			}
			num, size, err := getNumberOfSccAndMaxSizeScc()
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("Number of SCCs: %d, Max size: %d\n", num, size)
			}
		case "3": // find WCCs
			if ug == nil {
				fmt.Println("Load a graph first.")
				continue
			}
			wcc, err = getWCC(ug)
			if err != nil {
				fmt.Println("Error finding WCCs:", err)
				continue
			}
			fmt.Printf("Number of WCCs: %d, Size of largest WCC: %d\n", len(wcc), len(wcc[0]))
		case "4": // diameter of largest WCC (double sweep)
			if ug == nil || wcc == nil {
				fmt.Println("Run WCC calculation first (option 3).")
				continue
			}
			diameter, err := getDiameterDoubleSweep(ug, wcc[0])
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("Diameter of largest WCC (Double Sweep): %d\n", diameter)
			}
		case "5": // diameter of largest WCC (snowball + double sweep)
			if ug == nil || wcc == nil {
				fmt.Println("Run WCC calculation first (option 3).")
				continue
			}
			snowball, err := getSnowballGraph(ug, wcc[0])
			if err != nil {
				fmt.Println("Error:", err)
				continue
			}
			diameter, err := getDiameterDoubleSweep(snowball, snowball.GetNodesSlice())
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("Diameter of largest WCC (Double Sweep + Snowball): %d\n", diameter)
			}
		case "6": // percentile of distances in largest WCC
			if len(args) < 2 {
				fmt.Println("Usage: 6 <percentile>")
				continue
			}
			if ug == nil || wcc == nil {
				fmt.Println("Run WCC calculation first (option 3).")
				continue
			}
			p, err := strconv.ParseFloat(args[1], 64)
			if err != nil || p <= 0 || p > 1 {
				fmt.Println("Invalid percentile value. Must be between 0 and 1.")
				continue
			}
			percentile, err := getPercentile(p, ug, wcc[0])
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("%.2f percentile distance: %.2f\n", p, percentile)
			}
		case "7": // percentile of distances in largest WCC (Snowball)
			if len(args) < 2 {
				fmt.Println("Usage: 7 <percentile>")
				continue
			}
			if ug == nil || wcc == nil {
				fmt.Println("Run WCC calculation first (option 3).")
				continue
			}
			p, err := strconv.ParseFloat(args[1], 64)
			if err != nil || p <= 0 || p > 1 {
				fmt.Println("Invalid percentile value. Must be between 0 and 1.")
				continue
			}
			snowball, err := getSnowballGraph(ug, wcc[0])
			if err != nil {
				fmt.Println("Error:", err)
				continue
			}
			percentile, err := getPercentile(p, snowball, snowball.GetNodesSlice())
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("%.2f percentile distance (Snowball): %.2f\n", p, percentile)
			}
		case "8": // count triangles
			if ug == nil {
				fmt.Println("Load a graph first.")
				continue
			}
			if triangles == -1 {
				triangles, err = getTriangles(ug)
				if err != nil {
					fmt.Println("Error:", err)
					continue
				}
			}
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("Number of triangles: %d\n", triangles)
			}
		case "9": // average clustering coefficient
			if ug == nil || wcc == nil {
				fmt.Println("Run WCC calculation first (option 3).")
				continue
			}
			val, err := getAverageClusteringCoefficient(ug, nil)
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("Average clustering coefficient: %.4f\n", val)
			}
		case "10": // average clustering coefficient (largest WCC)
			if ug == nil || wcc == nil {
				fmt.Println("Run WCC calculation first (option 3).")
				continue
			}
			val, err := getAverageClusteringCoefficient(ug, wcc[0])
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("Average clustering coefficient (largest WCC): %.4f\n", val)
			}
		case "11": // global clustering coefficient
			if ug == nil {
				fmt.Println("Load a graph first.")
				continue
			}

			if triangles == -1 {
				triangles, err = getTriangles(ug)
				if err != nil {
					fmt.Println("Error:", err)
					continue
				}
			}

			val, err := getGlobalClusteringCoefficient(ug, triangles)
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("Global clustering coefficient: %.4f\n", val)
			}
		case "12": // local clustering coefficient
			if len(args) < 2 {
				fmt.Println("Usage: 12 <node_id>")
				continue
			}
			if ug == nil {
				fmt.Println("Load a graph first.")
				continue
			}
			node, err := strconv.Atoi(args[1])
			if err != nil {
				fmt.Println("Error:", err)
				continue
			}
			res, err := ug.GetLocalClusteringCoefficient(graph.Node(node))
			if err != nil {
				fmt.Println("Error:", err)
				continue
			}
			fmt.Printf("Local clustering coefficient for node %d: %.4f\n", node, res)
		case "13": // degrees
			if ug == nil {
				fmt.Println("Load a graph first.")
				continue
			}
			minD, avgD, maxD, err := getDegreeDistribution(ug, "")
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("Degree stats - Min: %d, Avg: %.2f, Max: %d\n", minD, avgD, maxD)
			}
		case "14": // largest WCC size ration after node removals
			if len(args) < 2 {
				fmt.Println("Usage: 14 <percent to remove>")
				continue
			}
			if ug == nil {
				fmt.Println("Load a graph first.")
				continue
			}
			p, err := strconv.ParseFloat(args[1], 64)
			if err != nil || p <= 0 || p >= 1 {
				fmt.Println("Invalid percentile value. Must be between 0 and 1.")
				continue
			}
			randRatio, maxDegRatio, err := getWCCRatiosAfterRemovals(ug, p)
			if err != nil {
				fmt.Println("Error:", err)
			} else {
				fmt.Printf("Largest WCC size ratio after random removals: %.4f\n", randRatio)
				fmt.Printf("Largest WCC size ratio after max-degree removals: %.4f\n", maxDegRatio)
			}
		case "15":
			if len(args) < 3 {
				fmt.Println("Usage: 15 <node_id> <node_id>")
			}
			if ug == nil {
				fmt.Println("Load a graph first.")
				continue
			}
			s, err := strconv.Atoi(args[1])
			if err != nil {
				fmt.Println("Error: ", err)
				continue
			}
			t, err := strconv.Atoi(args[2])
			if err != nil {
				fmt.Println("Error: ", err)
				continue
			}
			fmt.Println("Choose selection method:")
			fmt.Println("1 - Select random nodes")
			fmt.Println("2 - Select highest degree nodes")
			fmt.Println("3 - Select best coverage")

			fmt.Print("> ")
			method, err := reader.ReadString('\n')
			if err != nil {
				fmt.Printf("Error reading input: %v\n", err)
				continue
			}

			method = strings.TrimSpace(method)

			var selectFunction func(*graph.Graph, int) ([]graph.Node, error)
			switch method {
			case "1":
				selectFunction = landmarkAlgo.SelectRandomNodes
			case "2":
				selectFunction = landmarkAlgo.SelectHighestDegree
			case "3":
				selectFunction = landmarkAlgo.SelectBestCoverage
			default:
				fmt.Printf("Unknown method %s", method)
				continue
			}

			fmt.Println("Choose landmark algorithm:")
			fmt.Println("1 - Basic Landmark")
			fmt.Println("2 - Shortcut Landmark")
			fmt.Print("> ")
			algorithm, err := reader.ReadString('\n')
			if err != nil {
				fmt.Printf("Error reading input: %v\n", err)
				continue
			}

			algorithm = strings.TrimSpace(algorithm)

			fmt.Print("Input number of landmarks (1-1000): ")
			number, err := reader.ReadString('\n')
			if err != nil {
				fmt.Printf("Error reading input: %v\n", err)
				continue
			}

			number = strings.TrimSpace(number)

			numberInt, err := strconv.Atoi(number)
			if err != nil {
				fmt.Println("Error: ", err)
				continue
			}

			if numberInt < 1 || numberInt > 1000 {
				fmt.Println("Invalid number of landmarks: ", numberInt)
				continue
			}

			filePath, err := getLandmarkFilepath(graphName, algorithm)
			if err != nil {
				fmt.Println("Error: ", err)
				continue
			}

			var dist int

			err = func() error {
				defer os.Remove(filePath)

				switch algorithm {
				case "1":
					err := landmarkAlgo.PrecomputeLandmarks(ug, filePath, selectFunction, numberInt)
					if err != nil {
						return err
					}
					dist, err = landmarkAlgo.LandmarkBasic(filePath, int32(s), int32(t))
					if err != nil {
						return err
					}
				case "2":
					err := landmarkAlgo.PrecomputeLandmarksWithPaths(ug, filePath, selectFunction, numberInt)
					if err != nil {
						return err
					}
					dist, err = landmarkAlgo.LandmarkShortcut(ug, filePath, int32(s), int32(t))
					if err != nil {
						return err
					}
				default:
					fmt.Println("Unknown algorithm")
					return fmt.Errorf("unknown algorithm: %s", algorithm)
				}
				return nil
			}()
			if err != nil {
				fmt.Println("Error:", err)
				continue
			}

			fmt.Printf("Approx distance between %d and %d: %d\n", s, t, dist)

		case "99":
			fmt.Println("Exiting...")
			return
		default:
			fmt.Printf("Unknown command: %s\n", cmd)
		}
	}

}

func printHelp() {
	fmt.Println("Available commands:")
	fmt.Println(" 0                           - help")
	fmt.Println(" 1 <graph_type> <graph_name> - load graph from file")
	fmt.Println(" 2                           - number and size of SCC")
	fmt.Println(" 3                           - number and size of WCC")
	fmt.Println(" 4                           - diameter of largest WCC (Double Sweep)")
	fmt.Println(" 5                           - diameter of largest WCC (Snowball + Double Sweep)")
	fmt.Println(" 6 <percentile>              - percentile of distances in largest WCC")
	fmt.Println(" 7 <percentile>              - percentile of distances in largest WCC (Snowball)")
	fmt.Println(" 8                           - count triangles")
	fmt.Println(" 9                           - average clustering coefficient")
	fmt.Println("10                           - average clustering coefficient (largest WCC)")
	fmt.Println("11                           - global clustering coefficient")
	fmt.Println("12 <node_id>                 - local clustering coefficient for node <node_id>")
	fmt.Println("13                           - degree distribution")
	fmt.Println("14 <percent>                 - WCC ratio after x% node removals")
	fmt.Println("15 <node_id> <node_id>       - approx distance between two nodes")
	fmt.Println("99                           - exit")
}

func loadGraph(filePath string) (*graph.Graph, error) {
	graphName := tools.GetFileNameWithoutExt(filePath)
	auxPath := tools.GetFileDestination(filePath) + "aux_graphs/"
	auxSortedGraph = auxPath + graphName + "-sorted.txt"
	auxInvertedGraph = auxPath + graphName + "-inverted.txt"
	auxUndirectedGraph = auxPath + graphName + "-undirected.txt"
	if _, err := os.Stat(auxSortedGraph); errors.Is(err, os.ErrNotExist) {
		err := tools.SortNodesInFile(filePath, auxSortedGraph)
		if err != nil {
			return nil, fmt.Errorf("error in SortNodesInFile(): %w", err)
		}
	}
	if _, err := os.Stat(auxInvertedGraph); errors.Is(err, os.ErrNotExist) {
		err := tools.InvertEdgesInFile(filePath, auxInvertedGraph)
		if err != nil {
			return nil, fmt.Errorf("error in InvertEdgesInFile(): %w", err)
		}
	}
	if _, err := os.Stat(auxUndirectedGraph); errors.Is(err, os.ErrNotExist) {
		err := tools.UndirectEdgesInFile(filePath, auxUndirectedGraph)
		if err != nil {
			return nil, fmt.Errorf("error in UndirectEdgesInFile(): %w", err)
		}
	}
	ug, err := graph.FromFile(auxUndirectedGraph, false)
	if err != nil {
		return nil, fmt.Errorf("error reading graph from file: %w", err)
	}
	return ug, nil
}

func getSnowballGraph(g *graph.Graph, base []graph.Node) (*graph.Graph, error) {
	snowballGraph, err := graph.GetSnowballGraph(g, base, SnowballSize)
	if err != nil {
		return nil, fmt.Errorf("error building snowball: %w", err)
	}
	return snowballGraph, nil
}

func getNumberOfSccAndMaxSizeScc() (int, int, error) {
	g, err := graph.FromFile(auxSortedGraph, true)
	if err != nil {
		return 0, 0, fmt.Errorf("error reading graph from file: %w", err)
	}
	scc, err := g.FindSCC(auxInvertedGraph)
	if err != nil {
		return 0, 0, fmt.Errorf("error finding SCC: %w", err)
	}

	scc = graph.SortComponents(scc, true)
	return len(scc), len(scc[0]), nil
}

func getWCC(g *graph.Graph) ([][]graph.Node, error) {
	wcc, err := g.FindWCC()
	if err != nil {
		return nil, fmt.Errorf("error finding WCC: %v", err)
	}
	return graph.SortComponents(wcc, true), nil
}

func getDiameterDoubleSweep(g *graph.Graph, component []graph.Node) (int, error) {
	randomNode := component[rand.IntN(len(component))]
	maxWCCDiameterTDS := g.GetDiameterDoubleSweep(randomNode)
	return maxWCCDiameterTDS, nil
}

func getPercentile(percentile float64, g *graph.Graph, nodes []graph.Node) (float64, error) {
	val, err := g.GetDistancePercentile(nodes, percentile, PercentileSampleSize)
	if err != nil {
		return 0, fmt.Errorf("error calculating percentile: %w", err)
	}
	return val, nil
}

func getTriangles(g *graph.Graph) (int64, error) {
	val, err := g.TrianglesNumber()
	if err != nil {
		return 0, fmt.Errorf("error counting triangles: %w", err)
	}
	return val, nil
}

func getAverageClusteringCoefficient(g *graph.Graph, nodes []graph.Node) (float64, error) {
	val, err := g.GetAverageClusteringCoefficient(nodes)
	if err != nil {
		return 0, fmt.Errorf("error calculating average clustering coefficient: %w", err)
	}
	return val, nil
}

func getGlobalClusteringCoefficient(g *graph.Graph, triangles int64) (float64, error) {
	val, err := g.GetGlobalClusteringCoefficient(triangles)
	if err != nil {
		return 0, fmt.Errorf("error calculating global clustering coefficient: %w", err)
	}
	return val, nil
}

func getDegreeDistribution(g *graph.Graph, filepath string) (int, float64, int, error) {
	minD, avgD, maxD, err := g.ProcessNodesDegrees(filepath)
	if err != nil {
		return 0, 0, 0, fmt.Errorf("error processing nodes degrees: %w", err)
	}
	return minD, avgD, maxD, nil
}

func getWCCRatiosAfterRemovals(
	ugraph *graph.Graph,
	percent float64,
) (float64, float64, error) {
	var randomResult, maxDegResult float64

	var wg sync.WaitGroup

	wg.Add(2)

	go func() {
		defer wg.Done()
		ugraphCopy := ugraph.DeepCopy()
		ugraphCopy.RemoveRandomNodes(percent)
		wcc, err := getWCC(ugraphCopy)
		if err != nil {
			fmt.Printf("Error in getWCC: %v\n", err)
			return
		}
		ratio := float64(len(wcc[0])) / float64(ugraphCopy.NumberOfNodes())
		randomResult = ratio
	}()

	go func() {
		defer wg.Done()
		ugraphCopy := ugraph.DeepCopy()
		ugraphCopy.RemoveHighestDegreeNodes(percent)
		wcc, err := getWCC(ugraphCopy)
		if err != nil {
			fmt.Printf("Error in getWCC: %v\n", err)
			return
		}
		ratio := float64(len(wcc[0])) / float64(ugraphCopy.NumberOfNodes())
		maxDegResult = ratio
	}()

	wg.Wait()

	return randomResult, maxDegResult, nil
}
