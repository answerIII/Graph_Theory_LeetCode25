package graph

import (
	"log"
	"math/rand/v2"
	"testing"
)

const FILEPATH = "../datasets/directed/web-Google.txt"

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
		percentile90, err := GetDistancePercentile(
			wcc[0],
			graph.Adj,
			0.9,
			500,
		)
		if err != nil {
			t.Fatal(err)
		}
		t.Logf("90-percentile in largest WCC: %2.f", percentile90)
	})
}
