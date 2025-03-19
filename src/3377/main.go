package _3377

import (
	"container/heap"
	"slices"
	"strconv"
)

const N = 10001

var primes = slices.Repeat([]bool{true}, N)

func sieve() {
	primes[1] = false
	for i := 2; i < N; i++ {
		if primes[i] {
			for j := 2 * i; j < N; j += i {
				primes[j] = false
			}
		}
	}
}

type pqItem struct {
	steps int
	sums  int
}

type minHeap []pqItem

func (h minHeap) Len() int           { return len(h) }
func (h minHeap) Less(i, j int) bool { return h[i].steps < h[j].steps }
func (h minHeap) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }

func (h *minHeap) Push(x interface{}) {
	*h = append(*h, x.(pqItem))
}

func (h *minHeap) Pop() interface{} {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[:n-1]
	return x
}

func changeDigits(s []rune, h *minHeap, visited map[int]bool, sums int) {
	checkAndPush := func() {
		next, _ := strconv.Atoi(string(s))
		if !primes[next] && !visited[next] {
			heap.Push(h, pqItem{steps: sums + next, sums: next})
		}
	}

	for i := 0; i < len(s); i++ {
		tmp := s[i]

		if s[i] < '9' {
			s[i]++
			checkAndPush()
			s[i] = tmp
		}

		if s[i] > '0' && !(i == 0 && s[i] == '1') {
			s[i]--
			checkAndPush()
			s[i] = tmp
		}
	}
}

func minOperations(n int, m int) int {
	sieve()

	h := &minHeap{}
	heap.Init(h)
	visited := make(map[int]bool)

	heap.Push(h, pqItem{steps: n, sums: n})
	for h.Len() > 0 {
		item := heap.Pop(h).(pqItem)
		sums, curr := item.steps, item.sums

		if visited[curr] {
			continue
		}
		visited[curr] = true

		if curr == m {
			return sums
		}

		changeDigits([]rune(strconv.Itoa(curr)), h, visited, sums)
	}

	return -1
}
