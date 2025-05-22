package graph

import "testing"

func TestGraph_ProcessNodesDegrees(t *testing.T) {
	type args struct {
		filePath string
	}
	tests := []struct {
		name       string
		graphPath  string
		args       args
		wantMinDeg int
		wantAvgDeg float64
		wantMaxDeg int
		wantErr    bool
	}{
		{
			name:       "Graph wiki-vote",
			graphPath:  ugraphWikiVoteFilepath,
			args:       args{filePath: ""},
			wantMinDeg: 1,
			wantAvgDeg: 28.32,
			wantMaxDeg: 1065,
			wantErr:    false,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			graph, err := FromFile(tt.graphPath, false)
			if err != nil {
				t.Errorf("Error reading graph file: %v\n", err)
			}
			gotMinDeg, gotAvgDeg, gotMaxDeg, err := graph.ProcessNodesDegrees(tt.args.filePath)
			if (err != nil) != tt.wantErr {
				t.Errorf("ProcessNodesDegrees() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if gotMinDeg != tt.wantMinDeg {
				t.Errorf("ProcessNodesDegrees() gotMinDeg = %v, want %v", gotMinDeg, tt.wantMinDeg)
			}
			if roundFloat(gotAvgDeg, 2) != roundFloat(tt.wantAvgDeg, 2) {
				t.Errorf("ProcessNodesDegrees() gotAvgDeg = %v, want %v", roundFloat(gotAvgDeg, 2), tt.wantAvgDeg)
			}
			if gotMaxDeg != tt.wantMaxDeg {
				t.Errorf("ProcessNodesDegrees() gotMaxDeg = %v, want %v", gotMaxDeg, tt.wantMaxDeg)
			}
		})
	}
}
