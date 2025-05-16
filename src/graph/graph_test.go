package graph

import (
	"fmt"
	"log"
	"math/rand/v2"
	"strings"
	"testing"
)

const FILEPATH = "../datasets/very_large_graphs/com-orkut.ungraph.txt"

var graph *Graph

func init() {
	var err error
	graph, err = FromFile(FILEPATH, false)
	if err != nil {
		log.Fatalf("Error reading graph file: %v\n", err)
	}
}

func TestGraph_FindSCC(t *testing.T) {
	t.Run("Test with real data", func(t *testing.T) {
		got, err := graph.FindSCC()
		if err != nil {
			t.Errorf("FindSCC() error = %v", err)
			return
		}
		t.Logf("Number of SCC: %d", len(got))
	})
}

func TestGraph_GetDiameterDoubleSweep(t *testing.T) {
	t.Run("Test with real data", func(t *testing.T) {
		wcc, err := graph.FindWCC()
		if err != nil {
			t.Errorf("FindWCC() error = %v", err)
			return
		}
		wcc = SortComponents(wcc, true)
		t.Logf("Number of WCC: %d", len(wcc))
		n := len(wcc[0])
		if n > 1 {
			n--
		}
		randomNode := wcc[0][rand.IntN(n)]
		got := graph.GetDiameterDoubleSweep(randomNode)
		t.Logf("Diameter: %d", got)
	})
}

func TestGraph_GetDistancePercentile(t *testing.T) {
	t.Run("Subtest1", func(t *testing.T) {
		wcc, err := graph.FindWCC()
		if err != nil {
			t.Errorf("FindWCC() error = %v", err)
			return
		}
		wcc = SortComponents(wcc, true)
		percentile90, err := graph.GetDistancePercentile(wcc[0], 0.9, 500)
		if err != nil {
			t.Fatal(err)
		}
		t.Logf("90-percentile in largest WCC: %2.f", percentile90)
	})
}

func TestGraph_GetSnowballGraph(t *testing.T) {
	t.Run("Test Generate Snowball Graph", func(t *testing.T) {
		wcc, err := graph.FindWCC()
		if err != nil {
			t.Errorf("FindWCC() error = %v", err)
			return
		}
		wcc = SortComponents(wcc, true)

		subgraph, err := GetSnowballGraph(wcc[0], graph.Adj, 500)
		if err != nil {
			t.Errorf("GetSnowballGraph() error = %v", err)
			return
		}

		wcc, err = subgraph.FindWCC()
		if err != nil {
			t.Errorf("FindWCC() error in subgraph = %v", err)
			return
		}
		wcc = SortComponents(wcc, true)

		t.Logf("Subgraph info:\n")
		t.Logf("Number of nodes: %d\n", subgraph.NumberOfNodes())
		t.Logf("Number of edges: %d\n", subgraph.NumberOfEdges())
		randomNode := wcc[0][rand.IntN(len(wcc[0]))]
		got := subgraph.GetDiameterDoubleSweep(randomNode)
		t.Logf("Diameter: %d", got)
		percentile90, err := subgraph.GetDistancePercentile(wcc[0], 0.9, 500)
		if err != nil {
			t.Errorf("GetDistancePercentile() error in subgraph = %v", err)
			return
		}
		t.Logf("90 percentile: %.2f", percentile90)
	})
}

func TestGraph_TrianglesNumber(t *testing.T) {
	t.Run("Count triangles", func(t *testing.T) {
		n, err := graph.TrianglesNumber()
		if err != nil {
			t.Errorf("TrianglesNumber() error = %v", err)
			return
		}
		t.Logf("Number of triangles: %d", n)
	})
}

func TestGraph_GetAverageClusteringCoefficient(t *testing.T) {
	t.Run("Average clustering coefficient", func(t *testing.T) {
		avgCC, err := graph.GetAverageClusteringCoefficient(nil)
		if err != nil {
			t.Errorf("GetAverageClusteringCoefficient() error = %v", err)
			return
		}
		t.Logf("Average clustering coefficient: %.4f", avgCC)
	})
}

func TestGraph_GetGlobalClusteringCoefficient(t *testing.T) {
	t.Run("Global clustering coefficient", func(t *testing.T) {
		glbCC, err := graph.GetGlobalClusteringCoefficient(-1)
		if err != nil {
			t.Errorf("GetGlobalClusteringCoefficient() error = %v", err)
			return
		}
		t.Logf("Global clustering coefficient: %.4f", glbCC)
	})
}

func TestGraph_ProcessNodesDegrees(t *testing.T) {
	t.Run("Process nodes degrees", func(t *testing.T) {
		parts := strings.Split(FILEPATH, "/")
		path := fmt.Sprintf("../visualization/data/degrees-%s", parts[len(parts)-1])
		minD, avgD, maxD, err := graph.ProcessNodesDegrees(path)
		if err != nil {
			t.Errorf("ProcessNodesDegrees() error = %v", err)
			return
		}

		t.Logf("Min degree: %d", minD)
		t.Logf("Avg degree: %.2f", avgD)
		t.Logf("Max degree: %d", maxD)
	})
}
