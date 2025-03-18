package _3310

import (
	"reflect"
	"sort"
	"testing"
)

func Test_remainingMethods(t *testing.T) {
	type args struct {
		n           int
		k           int
		invocations [][]int
	}
	tests := []struct {
		name string
		args args
		want []int
	}{
		{
			name: "Test_1",
			args: args{
				n: 4,
				k: 1,
				invocations: [][]int{
					{1, 2}, {0, 1}, {3, 2},
				},
			},
			want: []int{0, 1, 2, 3},
		},
		{
			name: "Test_2",
			args: args{
				n: 5,
				k: 0,
				invocations: [][]int{
					{1, 2}, {0, 2}, {0, 1}, {3, 4},
				},
			},
			want: []int{3, 4},
		},
		{
			name: "Test_3",
			args: args{
				n: 3,
				k: 2,
				invocations: [][]int{
					{1, 2}, {0, 1}, {2, 0},
				},
			},
			want: nil,
		},
		{
			name: "Test_4",
			args: args{
				n: 6,
				k: 0,
				invocations: [][]int{
					{3, 0},
					{0, 5},
					{0, 4},
					{5, 1},
					{0, 3},
					{5, 2},
				},
			},
			want: nil,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := remainingMethods(tt.args.n, tt.args.k, tt.args.invocations); !reflect.DeepEqual(got, tt.want) {
				sort.Ints(got)
				t.Errorf("remainingMethods() = %v, want %v", got, tt.want)
			}
		})
	}
}
