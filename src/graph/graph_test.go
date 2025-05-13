package graph

import (
	"log"
	"testing"
)

const FILEPATH = "../datasets/directed/web-Google.txt"

var graph *Graph

func init() {
	var err error
	graph, err = FromFile(FILEPATH, true)
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
