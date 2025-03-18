package _990

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

func equationsPossible(equations []string) bool {
	link := make([]int, 26)
	size := make([]int, 26)
	for i := 0; i < 26; i++ {
		link[i] = i
		size[i] = 1
	}

	for _, equation := range equations {
		if equation[1] == '=' {
			unite(int(equation[0]-'a'), int(equation[3]-'a'), link, size)
		}
	}

	for _, equation := range equations {
		if equation[1] == '!' {
			a, b := int(equation[0]-'a'), int(equation[3]-'a')
			if same(a, b, link) {
				return false
			}
		}
	}

	return true
}
