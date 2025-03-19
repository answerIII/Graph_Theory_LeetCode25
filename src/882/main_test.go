package _882

import "testing"

func Test_reachableNodes(t *testing.T) {
	type args struct {
		edges    [][]int
		maxMoves int
		n        int
	}
	tests := []struct {
		name string
		args args
		want int
	}{
		{
			name: "Test_1",
			args: args{
				edges: [][]int{
					{0, 1, 10}, {0, 2, 1}, {1, 2, 2},
				},
				maxMoves: 6,
				n:        3,
			},
			want: 13,
		},
		{
			name: "Test_2",
			args: args{
				edges: [][]int{
					{0, 1, 4}, {1, 2, 6}, {0, 2, 8}, {1, 3, 1},
				},
				maxMoves: 10,
				n:        4,
			},
			want: 23,
		},
		{
			name: "Test_3",
			args: args{
				edges: [][]int{
					{1, 2, 4}, {1, 4, 5}, {1, 3, 1}, {2, 3, 4}, {3, 4, 5},
				},
				maxMoves: 17,
				n:        5,
			},
			want: 1,
		},
		{
			name: "Test_4",
			args: args{
				edges: [][]int{
					{0, 3, 8}, {0, 1, 4}, {2, 4, 3}, {1, 2, 0}, {1, 3, 9}, {0, 4, 7}, {3, 4, 9}, {1, 4, 4}, {0, 2, 7}, {2, 3, 1},
				},
				maxMoves: 8,
				n:        5,
			},
			want: 40,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := reachableNodes(tt.args.edges, tt.args.maxMoves, tt.args.n); got != tt.want {
				t.Errorf("reachableNodes() = %v, want %v", got, tt.want)
			}
		})
	}
}
