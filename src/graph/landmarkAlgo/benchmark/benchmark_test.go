package benchmark

import (
	"fmt"
	"graph_theory/graph"
	"graph_theory/graph/landmarkAlgo"
	"math"
	"math/rand/v2"
	"os"
	"path/filepath"
	"reflect"
	"runtime"
	"sync/atomic"
	"testing"
	"time"
	"unsafe"
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
var selectType = SBC

func BenchmarkLandmarkBasic(b *testing.B) {
	start := time.Now()

	input := landmarkAlgo.DatasetInputPath(graphType, fmt.Sprintf("%s-undirected.txt", graphName))
	ugraph, err := graph.FromFile(input, false)
	if err != nil {
		b.Logf("ERROR: Can't read from %s: %v (took %d ms)", input, err, time.Since(start).Milliseconds())
		b.FailNow()
	}
	b.Logf("Graph successfully read (took %d ms)", time.Since(start).Milliseconds())

	nodesSlice := ugraph.GetNodesSlice()
	s := nodesSlice[rand.IntN(len(nodesSlice))]
	t := nodesSlice[rand.IntN(len(nodesSlice))]
	for s == t {
		t = nodesSlice[rand.IntN(len(nodesSlice))]
	}

	actualStart := time.Now()
	actual, err := getAccurateDistance(ugraph, int32(s), int32(t))
	if err != nil {
		b.Logf("ERROR: Failed to get accurate distance: %v (took %d ms)", err, time.Since(actualStart).Milliseconds())
		b.FailNow()
	}
	b.Logf("ACTUAL DISTANCE: %d (took %d ms)", actual, time.Since(actualStart).Milliseconds())

	nodesN := []int{64, 128, 256, 512}

	for _, nodes := range nodesN {
		nodes := nodes
		iterStart := time.Now()

		output := landmarkAlgo.DatasetOutputPath(graphType, fmt.Sprintf("%s-random-%d.bin", graphName, nodes))
		dir := filepath.Dir(output)

		if err = os.MkdirAll(dir, os.ModePerm); err != nil {
			b.Logf("ERROR: Can't create dirs: %v (took %d ms)", err, time.Since(iterStart).Milliseconds())
			b.FailNow()
		}

		var totalEstimatedDistance int64
		var totalRelativeError float64
		var iterCount int64

		selectName := runtime.FuncForPC(reflect.ValueOf(selectType).Pointer()).Name()

		b.Run(fmt.Sprintf("bench %s %s landmarks %d", graphName, selectName, nodes), func(b *testing.B) {
			precomputeStart := time.Now()
			b.Logf("Precomputing landmarks")

			err = landmarkAlgo.PrecomputeLandmarks(ugraph, output, selectType, nodes)
			if err != nil {
				b.Logf("ERROR: Precompute failed: %v (took %d ms)", err, time.Since(precomputeStart).Milliseconds())
				b.FailNow()
			}
			b.Logf("Precompute finished (took %d ms)", time.Since(precomputeStart).Milliseconds())

			benchmarkStart := time.Now()
			b.Logf("Benchmarking LandmarkBasic (%d %d)", s, t)

			for b.Loop() {
				estimated, err := landmarkAlgo.LandmarkBasic(output, int32(s), int32(t))
				if err != nil {
					b.Logf("ERROR: LandmarkBasic failed: %v", err)
					b.FailNow()
				}

				atomic.AddInt64(&totalEstimatedDistance, int64(estimated))
				atomic.AddInt64(&iterCount, 1)
				atomicAddFloat64(&totalRelativeError, math.Abs(float64(estimated-actual))/float64(actual))
			}
			b.Logf("Benchmark iteration finished (took %d ms)", time.Since(benchmarkStart).Milliseconds())
		})

		avgEstimatedDistance := float64(totalEstimatedDistance) / float64(iterCount)
		avgError := totalRelativeError / float64(iterCount)
		avgTime := float64(time.Since(iterStart).Milliseconds()) / float64(iterCount)

		b.Logf("RESULTS for %d landmarks:", nodes)
		b.Logf("  - Iterations: %d", iterCount)
		b.Logf("  - Avg estimated distance: %.2f", avgEstimatedDistance)
		b.Logf("  - Avg relative error: %.4f", avgError)
		b.Logf("  - Avg time: %.2f ms", avgTime)

		info, err := os.Stat(output)
		if err != nil {
			b.Logf("WARN: Failed to get file info for %s: %v", output, err)
		} else {
			b.Logf("Landmark file size: %.2f MB", float64(info.Size())/1024/1024)
		}

		//_ = os.Remove(output)
	}
}

func BenchmarkLandmarkShortcut(b *testing.B) {
	start := time.Now()

	input := landmarkAlgo.DatasetInputPath(graphType, fmt.Sprintf("%s-undirected.txt", graphName))
	ugraph, err := graph.FromFile(input, false)
	if err != nil {
		b.Logf("ERROR: Can't read from %s: %v (took %d ms)", input, err, time.Since(start).Milliseconds())
		b.FailNow()
	}
	b.Logf("Graph successfully readed (took %d ms)", time.Since(start).Milliseconds())

	nodesSlice := ugraph.GetNodesSlice()
	s := nodesSlice[rand.IntN(len(nodesSlice))]
	t := nodesSlice[rand.IntN(len(nodesSlice))]
	for s == t {
		t = nodesSlice[rand.IntN(len(nodesSlice))]
	}

	actualStart := time.Now()
	actual, err := getAccurateDistance(ugraph, int32(s), int32(t))
	if err != nil {
		b.Logf("ERROR: Failed to get accurate distance: %v (took %d ms)", err, time.Since(actualStart).Milliseconds())
		b.FailNow()
	}
	b.Logf("ACTUAL DISTANCE: %d (took %d ms)", actual, time.Since(actualStart).Milliseconds())

	nodesN := []int{64, 128, 256, 512}

	for _, nodes := range nodesN {
		nodes := nodes
		iterStart := time.Now()

		output := landmarkAlgo.DatasetOutputPath(graphType, fmt.Sprintf("%s-random-%d.txt", graphName, nodes))
		dir := filepath.Dir(output)

		if err = os.MkdirAll(dir, os.ModePerm); err != nil {
			b.Logf("ERROR: Can't create dirs: %v (took %d ms)", err, time.Since(iterStart).Milliseconds())
			b.FailNow()
		}

		var totalEstimatedDistance int64
		var totalRelativeError float64
		var iterCount int64

		selectName := runtime.FuncForPC(reflect.ValueOf(selectType).Pointer()).Name()

		b.Run(fmt.Sprintf("bench %s %s landmarks shortcut %d", graphName, selectName, nodes), func(b *testing.B) {
			precomputeStart := time.Now()
			b.Logf("Precomputing landmarks with paths")

			err = landmarkAlgo.PrecomputeLandmarksWithPaths(ugraph, output, selectType, nodes)
			if err != nil {
				b.Logf("ERROR: Precompute failed: %v (took %d ms)", err, time.Since(precomputeStart).Milliseconds())
				b.FailNow()
			}
			b.Logf("Precompute finished (took %d ms)", time.Since(precomputeStart).Milliseconds())

			benchmarkStart := time.Now()
			b.Logf("Benchmarking LandmarkShortcut (%d %d)", s, t)

			for b.Loop() {
				estimated, err := landmarkAlgo.LandmarkShortcut(ugraph, output, int32(s), int32(t))
				if err != nil {
					b.Logf("ERROR: LandmarkShortcut failed: %v", err)
					b.FailNow()
				}

				atomic.AddInt64(&totalEstimatedDistance, int64(estimated))
				atomic.AddInt64(&iterCount, 1)
				atomicAddFloat64(&totalRelativeError, math.Abs(float64(estimated-actual))/float64(actual))
			}
			b.Logf("Benchmark iteration finished (took %d ms)", time.Since(benchmarkStart).Milliseconds())
		})

		avgEstimatedDistance := float64(totalEstimatedDistance) / float64(iterCount)
		avgError := totalRelativeError / float64(iterCount)
		avgTime := float64(time.Since(iterStart).Milliseconds()) / float64(iterCount)

		b.Logf("RESULTS for %d landmarks:", nodes)
		b.Logf("  - Iterations: %d", iterCount)
		b.Logf("  - Avg estimated distance: %.2f", avgEstimatedDistance)
		b.Logf("  - Avg relative error: %.4f", avgError)
		b.Logf("  - Avg time: %.2f ms", avgTime)

		info, err := os.Stat(output)
		if err != nil {
			b.Logf("WARN: Failed to get file info for %s: %v", output, err)
		} else {
			b.Logf("Landmark file size: %.2f MB", float64(info.Size())/1024/1024)
		}

		//_ = os.Remove(output)
	}
}

func atomicAddFloat64(val *float64, delta float64) {
	for {
		old := *val
		newData := old + delta
		if atomic.CompareAndSwapUint64(
			(*uint64)(unsafe.Pointer(val)),
			math.Float64bits(old),
			math.Float64bits(newData),
		) {
			return
		}
	}
}
