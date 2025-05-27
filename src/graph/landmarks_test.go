package graph

import (
	"os"
	"testing"
)

func TestGraph_GenerateLandmarkFiles(t *testing.T) {
	type fields struct {
		undirectedGraphFilepath string
		method                  func(*Graph, int) ([]Node, error)
	}
	tests := []struct {
		name   string
		fields fields
	}{
		{
			name:   "Graph Wiki-Vote",
			fields: fields{ugraphWikiVoteFilepath, (*Graph).SelectRandomNodes},
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			graph, err := FromFile(tt.fields.undirectedGraphFilepath, false)
			if err != nil {
				t.Errorf("Error reading graph file: %v\n", err)
			}
			if err := os.MkdirAll("../datasets/directed/landmarks", 0o775); err != nil {
				t.Errorf("Error creating directory: %v\n", err)
			}
			if err := os.MkdirAll("../datasets/undirected/landmarks", 0o775); err != nil {
				t.Errorf("Error creating directory: %v\n", err)
			}
			if err := os.MkdirAll("../datasets/very_large_graphs/landmarks", 0o775); err != nil {
				t.Errorf("Error creating directory: %v\n", err)
			}
			err = graph.PrecomputeLandmarks(landmarkRandomWikiVoteFilepath, tt.fields.method, 50)
			if err != nil {
				t.Errorf("Error precomputing landmarks: %v\n", err)
			}
			err = graph.PrecomputeLandmarksWithPaths(landmarksPathsRandomWikiVoteFilepath, tt.fields.method, 50)
			if err != nil {
				t.Errorf("Error precomputing landmarks with paths: %v\n", err)
			}
		})
	}
}

func TestGraph_LandmarkBasic(t *testing.T) {
	type fields struct {
		landmarkPath string
		s            int
		t            int
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

func TestGraph_LandmarkShortcuts(t *testing.T) {
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
			name:   "Graph Wiki-Vote",
			fields: fields{ugraphWikiVoteFilepath, landmarksPathsRandomWikiVoteFilepath, 0, 1},
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			g, err := FromFile(tt.fields.undirectedGraphFilepath, false)
			if err != nil {
				t.Errorf("Error reading a graph: %v\n", err)
			}
			dist, err := g.LandmarkShortcut(tt.fields.landmarkWithPathsPath, tt.fields.s, tt.fields.t)
			if err != nil {
				t.Errorf("Error in LandmarkShortcut: %v\n", err)
			}
			t.Logf("Approximate dist from node %d to %d: %d", tt.fields.s, tt.fields.t, dist)
		})
	}
}
