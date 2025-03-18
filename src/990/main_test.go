package _990

import "testing"

func Test_equationsPossible(t *testing.T) {
	type args struct {
		equations []string
	}
	tests := []struct {
		name string
		args args
		want bool
	}{
		{
			name: "Test_1",
			args: args{
				equations: []string{"a==b", "b!=a"},
			},
			want: false,
		},
		{
			name: "Test_2",
			args: args{
				equations: []string{"b==a", "a==b"},
			},
			want: true,
		},
		{
			name: "Test_3",
			args: args{
				equations: []string{"e==d", "e==a", "f!=d", "b!=c", "a==b"},
			},
			want: true,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := equationsPossible(tt.args.equations); got != tt.want {
				t.Errorf("equationsPossible() = %v, want %v", got, tt.want)
			}
		})
	}
}
