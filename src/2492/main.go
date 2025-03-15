package _2492

import "math"

func find(x int, link []int) int {
	if x != link[x] {
		link[x] = find(link[x], link)
	}
	return link[x]
}

func unite(a int, b int, link []int, size []int) {
	a = find(a, link)
	b = find(b, link)

	if size[a] < size[b] {
		a, b = b, a
	}
	size[a] += size[b]
	link[b] = a
}

func same(a int, b int, link []int) bool {
	return find(a, link) == find(b, link)
}

func minScore(n int, roads [][]int) int {
	res := math.MaxInt32

	link := make([]int, n+1)
	size := make([]int, n+1)
	for i := 1; i < n+1; i++ {
		link[i] = i
		size[i] = 1
	}

	for _, road := range roads {
		unite(road[0], road[1], link, size)
	}

	for _, road := range roads {
		if same(road[0], 1, link) {
			res = min(res, road[2])
		}
	}

	return res
}
