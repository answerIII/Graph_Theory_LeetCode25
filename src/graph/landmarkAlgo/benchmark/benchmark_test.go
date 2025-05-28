package benchmark

import (
	"fmt"
	"graph_theory/graph"
	"graph_theory/graph/landmarkAlgo"
	"log"
	"math"
	"math/rand/v2"
	"os"
	"path/filepath"
	"reflect"
	"runtime"
	"testing"
)

const (
	directed   = "directed"
	undirected = "undirected"
	large      = "very_large_graphs"
)

const (
	socWikiVote = "soc-wiki-Vote"
	google      = "web-Google"
	notreDame   = "web-NotreDame"
	stanford    = "web-Stanford"
	wikiVote    = "Wiki-Vote"

	astro     = "CA-AstroPh"
	coauthors = "ca-coauthors-dblp"
	grqc      = "CA-GrQc"
	email     = "Email-EuAll"
	git       = "musae_git_edges"

	orkut   = "com-orkut.ungraph"
	youtube = "com-youtube.ungraph"
	vk      = "vk"
)

var (
	SRN = landmarkAlgo.SelectRandomNodes
	SHD = landmarkAlgo.SelectHighestDegree
	SBC = landmarkAlgo.SelectBestCoverage
)

var graphType = directed
var graphName = wikiVote
var selectType = SRN

func BenchmarkLandmarkBasic(b *testing.B) {
	input := landmarkAlgo.DatasetInputPath(graphType, fmt.Sprintf("%s-undirected.txt", graphName))

	ugraph, err := graph.FromFile(input, false)
	if err != nil {
		panic(fmt.Errorf("can't read from %s", input))
	}

	nodesSlice := ugraph.GetNodesSlice()

	s := nodesSlice[rand.IntN(len(nodesSlice))]
	t := nodesSlice[rand.IntN(len(nodesSlice))]
	for s == t {
		t = nodesSlice[rand.IntN(len(nodesSlice))]
	}

	actual, err := getAccurateDistance(ugraph, int32(s), int32(t))
	if err != nil {
		panic(err)
	}
	log.Printf("ACTUAL DISTANCE: %d", actual)

	nodesN := []int{64, 128, 256}

	for _, nodes := range nodesN {
		nodes := nodes

		output := landmarkAlgo.DatasetOutputPath(graphType, fmt.Sprintf("%s-random-%d.bin", graphName, nodes))
		dir := filepath.Dir(output)

		if err = os.MkdirAll(dir, os.ModePerm); err != nil {
			b.Fatalf("Can't create dirs: %v", err)
		}

		var totalEstimatedDistance int
		var totalRelativeError float64
		var iterCount int

		selectName := runtime.FuncForPC(reflect.ValueOf(selectType).Pointer()).Name()
		b.Run(fmt.Sprintf("bench %s %s landmarks %d", graphName, selectName, nodes), func(b *testing.B) {
			iterCount = b.N
			err = landmarkAlgo.PrecomputeLandmarks(ugraph, output, selectType, nodes)
			if err != nil {
				b.Fatalf("Precompute failed: %v", err)
			}

			for i := 0; i < b.N; i++ {
				estimated, err := landmarkAlgo.LandmarkBasic(output, int32(s), int32(t))
				if err != nil {
					b.Fatalf("LandmarkBasic failed: %v", err)
				}
				totalEstimatedDistance += estimated
				totalRelativeError += math.Abs(float64(estimated-actual)) / float64(actual)
			}
		})

		avgEstimatedDistance := float64(totalEstimatedDistance) / float64(iterCount)
		avgError := totalRelativeError / float64(iterCount)

		log.Printf("Avg estimated distance for %d landmarks: %.2f", nodes, avgEstimatedDistance)
		log.Printf("Avg relative error for %d landmarks: %.4f", nodes, avgError)

		info, err := os.Stat(output)
		if err != nil {
			log.Printf("failed to get file info for %s: %v", output, err)
		} else {
			log.Printf("landmark file size for %d landmarks: %.2f MB", nodes, float64(info.Size())/1024/1024)
		}

		_ = os.Remove(output)
	}
}

func BenchmarkLandmarkShortcut(b *testing.B) {
	input := landmarkAlgo.DatasetInputPath(graphType, fmt.Sprintf("%s-undirected.txt", graphName))

	ugraph, err := graph.FromFile(input, false)
	if err != nil {
		panic(fmt.Errorf("can't read from %s", input))
	}

	nodesSlice := ugraph.GetNodesSlice()

	s := nodesSlice[rand.IntN(len(nodesSlice))]
	t := nodesSlice[rand.IntN(len(nodesSlice))]
	for s == t {
		t = nodesSlice[rand.IntN(len(nodesSlice))]
	}

	actual, err := getAccurateDistance(ugraph, int32(s), int32(t))
	if err != nil {
		panic(err)
	}
	log.Printf("ACTUAL DISTANCE: %d", actual)

	nodesN := []int{64, 128, 256}

	for _, nodes := range nodesN {
		nodes := nodes

		output := landmarkAlgo.DatasetOutputPath(graphType, fmt.Sprintf("%s-random-%d.bin", graphName, nodes))
		dir := filepath.Dir(output)

		if err = os.MkdirAll(dir, os.ModePerm); err != nil {
			b.Fatalf("Can't create dirs: %v", err)
		}

		var totalEstimatedDistance int
		var totalRelativeError float64
		var iterCount int

		selectName := runtime.FuncForPC(reflect.ValueOf(selectType).Pointer()).Name()
		b.Run(fmt.Sprintf("bench %s %s landmarks shortcut %d", graphName, selectName, nodes), func(b *testing.B) {
			iterCount = b.N
			err = landmarkAlgo.PrecomputeLandmarks(ugraph, output, selectType, nodes)
			if err != nil {
				b.Fatalf("Precompute failed: %v", err)
			}

			for i := 0; i < b.N; i++ {
				estimated, err := landmarkAlgo.LandmarkShortcut(ugraph, output, int(s), int(t))
				if err != nil {
					b.Fatalf("LandmarkBasic failed: %v", err)
				}
				totalEstimatedDistance += estimated
				totalRelativeError += math.Abs(float64(estimated-actual)) / float64(actual)
			}
		})

		avgEstimatedDistance := float64(totalEstimatedDistance) / float64(iterCount)
		avgError := totalRelativeError / float64(iterCount)

		log.Printf("Avg estimated distance for %d landmarks: %.2f", nodes, avgEstimatedDistance)
		log.Printf("Avg relative error for %d landmarks: %.4f", nodes, avgError)

		info, err := os.Stat(output)
		if err != nil {
			log.Printf("failed to get file info for %s: %v", output, err)
		} else {
			log.Printf("landmark file size for %d landmarks: %.2f MB", nodes, float64(info.Size())/1024/1024)
		}

		_ = os.Remove(output)
	}
}
