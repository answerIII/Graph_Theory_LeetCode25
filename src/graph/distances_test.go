package graph

import (
	"math/rand/v2"
	"testing"
)

func TestGraph_GetDiameterDoubleSweep(t *testing.T) {
	type fields struct {
		sortedGraphFilepath string
	}
	tests := []struct {
		name   string
		fields fields
		want   int
	}{
		{
			name:   "Graph wiki-vote",
			fields: fields{wikiVoteFilepath},
			want:   7,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			graph, err := FromFile(tt.fields.sortedGraphFilepath, false)
			if err != nil {
				t.Errorf("Error reading graph file: %v\n", err)
			}
			wcc, err := graph.FindWCC()
			if err != nil {
				t.Errorf("FindWCC() error = %v", err)
				return
			}
			wcc = SortComponents(wcc, true)
			n := len(wcc[0])
			if n > 1 {
				n--
			}
			randomNode := wcc[0][rand.IntN(n)]

			if got := graph.GetDiameterDoubleSweep(randomNode); got != tt.want {
				t.Errorf("GetDiameterDoubleSweep() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGraph_GetDistancePercentile(t *testing.T) {
	type fields struct {
		sortedGraphFilepath string
	}
	type args struct {
		percentile float64
		sampleN    int
	}
	tests := []struct {
		name    string
		fields  fields
		args    args
		want    float64
		wantErr bool
	}{
		{
			name:   "Graph wiki-vote",
			fields: fields{wikiVoteFilepath},
			args:   args{percentile: 0.9, sampleN: 500},
			want:   4.0,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			graph, err := FromFile(tt.fields.sortedGraphFilepath, false)
			if err != nil {
				t.Errorf("Error reading graph file: %v\n", err)
			}

			wcc, err := graph.FindWCC()
			if err != nil {
				t.Errorf("FindWCC() error = %v", err)
				return
			}
			wcc = SortComponents(wcc, true)

			got, err := graph.GetDistancePercentile(wcc[0], tt.args.percentile, tt.args.sampleN)
			if (err != nil) != tt.wantErr {
				t.Errorf("GetDistancePercentile() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if roundFloat(got, 2) != roundFloat(tt.want, 2) {
				t.Errorf("GetDistancePercentile() got = %v, want %v", roundFloat(got, 2), roundFloat(tt.want, 2))
			}
		})
	}
}
