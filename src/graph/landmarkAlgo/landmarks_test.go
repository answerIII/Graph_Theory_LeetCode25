package landmarkAlgo

import (
	"graph_theory/graph"
	"testing"
)

const (
	ugraphWikiVoteFilepath = "../../datasets/directed/aux_graphs/Wiki-Vote-undirected.txt"

	landmarkRandomWikiVoteFilepath       = "./datasets/directed/Wiki-Vote-random.txt"
	landmarksPathsRandomWikiVoteFilepath = "./datasets/directed/Wiki-Vote-random-paths.txt"
)

func TestLandmarkBasic(t *testing.T) {
	type fields struct {
		landmarkPath string
		s            int32
		t            int32
	}
	tests := []struct {
		name   string
		fields fields
	}{
		{
			name:   "Graph Wiki-Vote",
			fields: fields{landmarkRandomWikiVoteFilepath, 0, 1},
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			dist, err := LandmarkBasic(tt.fields.landmarkPath, tt.fields.s, tt.fields.t)
			if err != nil {
				t.Errorf("Error in LandmarkBasic: %v\n", err)
			}
			t.Logf("Approximate dist from node %d to %d: %d", tt.fields.s, tt.fields.t, dist)
		})
	}
}

func TestLandmarkShortcuts(t *testing.T) {
	type fields struct {
		undirectedGraphFilepath string
		landmarkWithPathsPath   string
		s                       int
		t                       int
	}
	tests := []struct {
		name   string
		fields fields
	}{
		{
			name: "Graph Wiki-Vote",
			fields: fields{
				ugraphWikiVoteFilepath,
				landmarksPathsRandomWikiVoteFilepath,
				0, 1},
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			g, err := graph.FromFile(tt.fields.undirectedGraphFilepath, false)
			if err != nil {
				t.Errorf("Error reading a graph: %v\n", err)
			}
			dist, err := LandmarkShortcut(g, tt.fields.landmarkWithPathsPath, tt.fields.s, tt.fields.t)
			if err != nil {
				t.Errorf("Error in LandmarkShortcut: %v\n", err)
			}
			t.Logf("Approximate dist from node %d to %d: %d", tt.fields.s, tt.fields.t, dist)
		})
	}
}
