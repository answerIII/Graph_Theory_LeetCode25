package graph

import (
	"math/rand/v2"
	"reflect"
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

func TestGraph_RemoveNodes(t *testing.T) {
	tests := []struct {
		name      string
		graphPath string
		nodes     map[Node]struct{}
		wantTo    []Node
		wantPtr   []int
	}{
		{
			name:      "Remove 1 2",
			graphPath: exampleFilepath,
			nodes:     map[Node]struct{}{1: {}, 2: {}},
			wantTo:    []Node{0, 3},
			wantPtr:   []int{0, 0, 0, 0, 0, 2},
		},
		{
			name:      "Remove 1 2 3",
			graphPath: exampleFilepath,
			nodes:     map[Node]struct{}{1: {}, 2: {}, 3: {}},
			wantTo:    []Node{0},
			wantPtr:   []int{0, 0, 0, 0, 0, 1},
		},
		{
			name:      "Remove 0 1 2 3 4",
			graphPath: exampleFilepath,
			nodes:     map[Node]struct{}{0: {}, 1: {}, 2: {}, 3: {}, 4: {}},
			wantTo:    []Node{},
			wantPtr:   []int{0, 0, 0, 0, 0, 0},
		},
		{
			name:      "Remove 1",
			graphPath: exampleFilepath,
			nodes:     map[Node]struct{}{1: {}},
			wantTo:    []Node{2, 0, 3},
			wantPtr:   []int{0, 1, 1, 1, 1, 3},
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			graph, err := FromFile(tt.graphPath, false)
			if err != nil {
				t.Errorf("Error reading graph file: %v\n", err)
			}

			graph.RemoveNodes(tt.nodes)
			if !reflect.DeepEqual(graph.Adj.ptr, tt.wantPtr) {
				t.Errorf("ptr slice updated incorrectly. got = %v, want %v", graph.Adj.ptr, tt.wantPtr)
			}
			if !reflect.DeepEqual(graph.Adj.to, tt.wantTo) {
				t.Errorf("to slice updated incorrectly. got = %v, want %v", graph.Adj.to, tt.wantTo)
			}
		})
	}
}
