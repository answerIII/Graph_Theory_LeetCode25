package _2492

import "testing"

func Test_minScore(t *testing.T) {
	type args struct {
		n     int
		roads [][]int
	}
	tests := []struct {
		name string
		args args
		want int
	}{
		{
			name: "Test_1",
			args: args{
				n: 4,
				roads: [][]int{
					{1, 2, 9}, {2, 3, 6}, {2, 4, 5}, {1, 4, 7},
				},
			},
			want: 5,
		},
		{
			name: "Test_2",
			args: args{
				n: 4,
				roads: [][]int{
					{1, 2, 2}, {1, 3, 4}, {3, 4, 7},
				},
			},
			want: 2,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := minScore(tt.args.n, tt.args.roads); got != tt.want {
				t.Errorf("minScore() = %v, want %v", got, tt.want)
			}
		})
	}
}
