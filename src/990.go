package main

import "fmt"

func main() {
	fmt.Println(equationsPossible([]string{"c==c", "f!=a", "f==b", "b==c", "c==a"}))
}

func equationsPossible(equations []string) bool {
	ds := NewSmallDisjointSet()

	for _, equation := range equations {
		if equation[1] == '!' {
			continue
		}
		firstRepresentative := ds.GetRepresentative(equation[0])
		secondRepresentative := ds.GetRepresentative(equation[3])
		ds.PutRepresentative(firstRepresentative, secondRepresentative)
	}

	for _, equation := range equations {
		if equation[1] == '=' {
			continue
		}
		if ds.GetRepresentative(equation[0]) == ds.GetRepresentative(equation[3]) {
			return false
		}
	}

	return true
}

type SmallDisjointSet struct {
	Items [128]byte
}

func NewSmallDisjointSet() SmallDisjointSet {
	ds := SmallDisjointSet{}
	var i byte
	for i < 128 {
		ds.Items[i] = i
		i++
	}
	return ds
}

func (ds *SmallDisjointSet) GetRepresentative(item byte) byte {
	if ds.Items[item] != item {
		ds.Items[item] = ds.GetRepresentative(ds.Items[item])
	}
	return ds.Items[item]
}

func (ds *SmallDisjointSet) PutRepresentative(item byte, representative byte) {
	ds.Items[item] = representative
}
