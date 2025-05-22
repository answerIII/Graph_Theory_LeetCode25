package graph

import (
	"testing"
)

func TestGraph_FindSCC(t *testing.T) {
	type args struct {
		invertedGraphPath string
	}
	tests := []struct {
		name      string
		graphPath string
		args      args
		want      int
		wantErr   bool
	}{
		{
			name:      "Graph wiki-vote",
			graphPath: wikiVoteFilepath,
			args:      args{invertedGraphPath: invertedWikiVoteFilepath},
			want:      5788,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			graph, err := FromFile(tt.graphPath, true)
			if err != nil {
				t.Errorf("Error reading graph file: %v\n", err)
			}
			got, err := graph.FindSCC(tt.args.invertedGraphPath)
			if (err != nil) != tt.wantErr {
				t.Errorf("FindSCC() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if len(got) != tt.want {
				t.Errorf("FindSCC() got = %v, want %v", len(got), tt.want)
			}
		})
	}
}
