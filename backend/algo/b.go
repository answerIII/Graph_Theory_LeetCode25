package algo

import (
	"math/rand"
	"sort"
	"time"

	"github.com/HikkMind/graph/structs"
)

func GetRandomVertexSet(graph *structs.Graph, setPercentage float32) map[int]struct{} {

	setPercentage /= 100
	vertexCount := graph.VertexCount
	vertexList := make([]int, vertexCount)
	setSize := int(float32(vertexCount) * setPercentage)

	ind := 0
	for vertex := range graph.AdjList {
		vertexList[ind] = vertex
		ind++
	}

	randLocal := rand.New(rand.NewSource(time.Now().UnixNano()))
	randLocal.Shuffle(len(vertexList), func(i, j int) {
		vertexList[i], vertexList[j] = vertexList[j], vertexList[i]
	})

	vertexSet := make(map[int]struct{}, 0)

	for i := range setSize {
		vertexSet[vertexList[i]] = struct{}{}
	}

	return vertexSet

}

func GetMaxDegreeVertexSet(graph *structs.Graph, setPercentage float32) map[int]struct{} {

	setPercentage /= 100
	vertexCount := len(graph.AdjList)
	// vertexCount := len(graph.AdjList)
	vertexList := make([][]int, vertexCount)
	setSize := int(float32(vertexCount) * setPercentage)

	ind := 0
	for vertex, adj := range graph.AdjList {
		vertexList[ind] = []int{vertex, len(adj)}
		ind++
	}
	// fmt.Println(vertexList)
	sort.Slice(vertexList, func(i, j int) bool {
		return vertexList[i][1] >= vertexList[j][1]
	})

	vertexSet := make(map[int]struct{}, 0)

	for i := range setSize {
		vertexSet[vertexList[i][0]] = struct{}{}
	}

	return vertexSet

}
