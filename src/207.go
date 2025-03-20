func canFinish(numCourses int, prerequisites [][]int) bool {

	sons := make([][]int, numCourses)
	degree_in := make([]int, numCourses)
	for _, v := range prerequisites {
		sons[v[0]] = append(sons[v[0]], v[1])
		degree_in[v[1]]++
	}

	origin_queue := make([]int, numCourses)
	start, end := 0, 0
	for i, v := range degree_in {
		if v == 0 {
			origin_queue[end] = i
			end++
		}
	}

	for start < end {
		for _, v := range sons[origin_queue[start]] {
			degree_in[v]--
			if degree_in[v] == 0 {
				origin_queue[end] = v
				end++
			}
		}
		start++
	}

	return (end == numCourses)
}