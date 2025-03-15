package _1557

import (
	"reflect"
	"testing"
)

func Test_findSmallestSetOfVertices(t *testing.T) {
	type args struct {
		n     int
		edges [][]int
	}
	tests := []struct {
		name string
		args args
		want []int
	}{
		{
			name: "Test_1",
			args: args{
				6,
				[][]int{{0, 1}, {0, 2}, {2, 5}, {3, 4}, {4, 2}},
			},
			want: []int{0, 3},
		},
		{
			name: "Test_2",
			args: args{
				5,
				[][]int{{0, 1}, {2, 1}, {3, 1}, {1, 4}, {2, 4}},
			},
			want: []int{0, 2, 3},
		},
		{
			name: "Test_3",
			args: args{
				2,
				[][]int{{0, 1}},
			},
			want: []int{0},
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := findSmallestSetOfVertices(tt.args.n, tt.args.edges); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("findSmallestSetOfVertices() = %v, want %v", got, tt.want)
			}
		})
	}
}
