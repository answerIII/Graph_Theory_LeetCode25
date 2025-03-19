package _3377

import "testing"

func Test_minOperations(t *testing.T) {
	type args struct {
		n int
		m int
	}
	tests := []struct {
		name string
		args args
		want int
	}{
		{
			name: "Test_1",
			args: args{
				n: 10,
				m: 12,
			},
			want: 85,
		},
		{
			name: "Test_2",
			args: args{
				n: 4,
				m: 8,
			},
			want: -1,
		},
		{
			name: "Test_3",
			args: args{
				n: 6,
				m: 2,
			},
			want: -1,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := minOperations(tt.args.n, tt.args.m); got != tt.want {
				t.Errorf("minOperations() = %v, want %v", got, tt.want)
			}
		})
	}
}
