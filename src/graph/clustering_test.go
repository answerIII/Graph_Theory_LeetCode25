package graph

import "testing"

func TestGraph_GetAverageClusteringCoefficient(t *testing.T) {
	type args struct {
		nodes []Node
	}
	tests := []struct {
		name       string
		uGraphPath string
		args       args
		want       float64
		wantErr    bool
	}{
		{
			name:       "Graph wiki-vote",
			uGraphPath: ugraphWikiVoteFilepath,
			args:       args{nodes: nil},
			want:       0.1409,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			graph, err := FromFile(tt.uGraphPath, false)
			if err != nil {
				t.Errorf("Error reading graph file: %v\n", err)
			}
			got, err := graph.GetAverageClusteringCoefficient(tt.args.nodes)
			if (err != nil) != tt.wantErr {
				t.Errorf("GetAverageClusteringCoefficient() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if got != tt.want {
				t.Errorf("GetAverageClusteringCoefficient() got = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGraph_GetGlobalClusteringCoefficient(t *testing.T) {
	type args struct {
		triangles int64
	}
	tests := []struct {
		name       string
		uGraphPath string
		args       args
		want       float64
		wantErr    bool
	}{
		{
			name:       "Graph wiki-vote",
			uGraphPath: ugraphWikiVoteFilepath,
			args:       args{triangles: -1},
			want:       0.1255,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			graph, err := FromFile(tt.uGraphPath, false)
			if err != nil {
				t.Errorf("Error reading graph file: %v\n", err)
			}
			got, err := graph.GetGlobalClusteringCoefficient(tt.args.triangles)
			if (err != nil) != tt.wantErr {
				t.Errorf("GetGlobalClusteringCoefficient() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if got != tt.want {
				t.Errorf("GetGlobalClusteringCoefficient() got = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestGraph_TrianglesNumber(t *testing.T) {
	type fields struct {
		uGraphFilepath string
	}
	tests := []struct {
		name    string
		fields  fields
		want    int64
		wantErr bool
	}{
		{
			name:   "Graph soc-wiki-vote",
			fields: fields{ugraphSocWikiVoteFilepath},
			want:   2119,
		},
		{
			name:   "Graph wiki-vote",
			fields: fields{ugraphWikiVoteFilepath},
			want:   608389,
		},
		{
			name:   "Graph web-NotreDame",
			fields: fields{ugraphWebNotreDameFilepath},
			want:   8910005,
		},
		{
			name:   "Graph CA-AstroPh",
			fields: fields{ugraphCaAstroPhFilepath},
			want:   1351441,
		},
		{
			name:   "Graph CA-GrQc",
			fields: fields{ugraphCaGrqcFilepath},
			want:   48260,
		},
		{
			name:   "Graph Email-EuAll",
			fields: fields{ugraphEmailEuAllFilepath},
			want:   267313,
		},
		{
			name:   "Graph Musae-Git-Edges",
			fields: fields{ugraphMusaeGitEdgesFilepath},
			want:   523810,
		},
		{
			name:   "Graph ca-coauthors-dblp",
			fields: fields{ugraphCaCoauthorsDblpFilepath},
			want:   444095058,
		},
		// processes 2.5 minutes
		//{
		//	name:   "Graph orkut",
		//	fields: fields{ugraphOrkkutFilepath},
		//	want:   627584181,
		//},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			graph, err := FromFile(tt.fields.uGraphFilepath, false)
			if err != nil {
				t.Errorf("Error reading graph file: %v\n", err)
			}
			got, err := graph.TrianglesNumber()
			if (err != nil) != tt.wantErr {
				t.Errorf("TrianglesNumber() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if got != tt.want {
				t.Errorf("TrianglesNumber() got = %v, want %v", got, tt.want)
			}
		})
	}
}
