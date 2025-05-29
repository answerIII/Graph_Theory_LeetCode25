package landmarkAlgo

import (
	"graph_theory/graph"
	"os"
	"testing"
)

func TestPrecomputeLandmarks(t *testing.T) {
	type fields struct {
		ugraphFilepath string
		method         func(*graph.Graph, int) ([]graph.Node, error)
	}
	tests := []struct {
		name   string
		fields fields
	}{
		{
			name: "Graph Wiki-Vote",
			fields: fields{
				ugraphWikiVoteFilepath,
				SelectBestCoverage,
			},
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			g, err := graph.FromFile(tt.fields.ugraphFilepath, false)
			if err != nil {
				t.Errorf("Error reading g file: %v\n", err)
			}
			if err := os.MkdirAll("./datasets/directed", 0o775); err != nil {
				t.Errorf("Error creating directory: %v\n", err)
			}
			if err := os.MkdirAll("./datasets/undirected/", 0o775); err != nil {
				t.Errorf("Error creating directory: %v\n", err)
			}
			if err := os.MkdirAll("./datasets/very_large_graphs", 0o775); err != nil {
				t.Errorf("Error creating directory: %v\n", err)
			}
			err = PrecomputeLandmarks(g, landmarkRandomWikiVoteFilepath, tt.fields.method, 500)
			if err != nil {
				t.Errorf("Error precomputing landmarks: %v\n", err)
			}
		})
	}
}

func TestPrecomputeLandmarksWithPaths(t *testing.T) {
	type fields struct {
		ugraphFilepath string
		method         func(*graph.Graph, int) ([]graph.Node, error)
	}
	tests := []struct {
		name   string
		fields fields
	}{
		{
			name: "Graph Wiki-Vote",
			fields: fields{
				ugraphWikiVoteFilepath,
				SelectRandomNodes,
			},
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			g, err := graph.FromFile(tt.fields.ugraphFilepath, false)
			if err != nil {
				t.Errorf("Error reading g file: %v\n", err)
			}
			if err := os.MkdirAll("./datasets/directed", 0o775); err != nil {
				t.Errorf("Error creating directory: %v\n", err)
			}
			if err := os.MkdirAll("./datasets/undirected", 0o775); err != nil {
				t.Errorf("Error creating directory: %v\n", err)
			}
			if err := os.MkdirAll("./datasets/very_large_graphs", 0o775); err != nil {
				t.Errorf("Error creating directory: %v\n", err)
			}
			err = PrecomputeLandmarksWithPaths(g, landmarksPathsRandomWikiVoteFilepath, tt.fields.method, 50)
			if err != nil {
				t.Errorf("Error precomputing landmarks with paths: %v\n", err)
			}
		})
	}
}
