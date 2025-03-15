package _2368

import "testing"

func Test_reachableNodes(t *testing.T) {
	type args struct {
		n          int
		edges      [][]int
		restricted []int
	}
	tests := []struct {
		name string
		args args
		want int
	}{
		{
			name: "Test_1",
			args: args{
				n: 7,
				edges: [][]int{
					{0, 1}, {1, 2}, {3, 1}, {4, 0}, {0, 5}, {5, 6},
				},
				restricted: []int{4, 5},
			},
			want: 4,
		},
		{
			name: "Test_2",
			args: args{
				n: 7,
				edges: [][]int{
					{0, 1}, {0, 2}, {0, 5}, {0, 4}, {3, 2}, {6, 5},
				},
				restricted: []int{4, 2, 1},
			},
			want: 3,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := reachableNodes(tt.args.n, tt.args.edges, tt.args.restricted); got != tt.want {
				t.Errorf("reachableNodes() = %v, want %v", got, tt.want)
			}
		})
	}
}
