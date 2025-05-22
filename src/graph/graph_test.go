package graph

import (
	"math/rand/v2"
	"testing"
)

func TestGetSnowballGraph(t *testing.T) {
	type args struct {
		snowBallSize int
	}
	tests := []struct {
		name       string
		uGraphPath string
		args       args
	}{
		{
			name:       "Graph wiki-vote",
			uGraphPath: ugraphWikiVoteFilepath,
			args:       args{snowBallSize: 500},
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			graph, err := FromFile(tt.uGraphPath, false)
			if err != nil {
				t.Errorf("Error reading graph file: %v\n", err)
			}

			wcc, err := graph.FindWCC()
			if err != nil {
				t.Errorf("FindWCC() error = %v", err)
				return
			}
			wcc = SortComponents(wcc, true)

			subgraph, err := GetSnowballGraph(graph, wcc[0], tt.args.snowBallSize)
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
}
