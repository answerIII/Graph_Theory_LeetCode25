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
)

var nodesN = []int{4, 8, 16, 32, 64, 128}
var selectTypes = []func(*graph.Graph, int) ([]graph.Node, error){
	landmarkAlgo.SelectRandomNodes,
	landmarkAlgo.SelectHighestDegree,
	landmarkAlgo.SelectBestCoverage,
}

const pathNum = 100

type Path struct {
	s graph.Node
	t graph.Node
}

func BenchmarkMainLandmarkBasic(b *testing.B) {
	graphType := directed

	graphNames := []string{socWikiVote}

	for _, graphName := range graphNames {
		start := time.Now()

		// Create log file
		logFileName := fmt.Sprintf("%s-landmarkBasic-bench.txt", graphName)
		logFileName = filepath.Join("output", logFileName)
		logFile, err := os.Create(logFileName)
		if err != nil {
			b.Logf("ERROR: Can't create log file: %v", err)
			b.FailNow()
		}

		input := landmarkAlgo.DatasetInputPath(graphType, fmt.Sprintf("%s-undirected.txt", graphName))
		ugraph, err := graph.FromFile(input, false)
		if err != nil {
			writeLog(logFile, "ERROR: Can't read from %s: %v (took %d ms)", input, err, time.Since(start).Milliseconds())
			b.FailNow()
		}

		nodesSlice := ugraph.GetNodesSlice()

		for _, selectType := range selectTypes {
			selectName := runtime.FuncForPC(reflect.ValueOf(selectType).Pointer()).Name()
			writeLog(logFile, "%s", selectName)

			for _, nodes := range nodesN {
				nodes := nodes

				output := landmarkAlgo.DatasetOutputPath(graphType, fmt.Sprintf("%s-random-%d.bin", graphName, nodes))
				dir := filepath.Dir(output)

				if err = os.MkdirAll(dir, os.ModePerm); err != nil {
					writeLog(logFile, "ERROR: Can't create dirs: %v ", err)
					b.FailNow()
				}

				used := map[Path]bool{}
				paths := make([]Path, 0, pathNum)

				for len(paths) < pathNum {
					s := nodesSlice[rand.IntN(len(nodesSlice))]
					t := nodesSlice[rand.IntN(len(nodesSlice))]
					for s == t {
						t = nodesSlice[rand.IntN(len(nodesSlice))]
					}
					if s > t {
						s, t = t, s
					}
					path := Path{s, t}
					if _, has := used[path]; has {
						continue
					}
					used[path] = true
					paths = append(paths, path)
				}

				var totalActualDistanceTime int64
				var totalEstimatedDistanceTime int64
				var totalAbsoluteError float64
				var iterCount int64

				b.Run(fmt.Sprintf("bench %s %s landmarks %d", graphName, selectName, nodes), func(b *testing.B) {
					precomputeStart := time.Now()
					writeLog(logFile, "landmarks: %d", nodes)

					err = landmarkAlgo.PrecomputeLandmarks(ugraph, output, selectType, nodes)
					if err != nil {
						writeLog(logFile, "ERROR: Precompute failed: %v (took %d ms)", err, time.Since(precomputeStart).Milliseconds())
						b.FailNow()
					}
					writeLog(logFile, "precomputing time: %d ms", time.Since(precomputeStart).Milliseconds())

					for b.Loop() {
						for _, path := range paths {
							actualStart := time.Now()
							actual, err := getAccurateDistance(ugraph, int32(path.s), int32(path.t))
							if err != nil {
								continue
							}
							atomic.AddInt64(&totalActualDistanceTime, time.Since(actualStart).Milliseconds())

							estimatedStart := time.Now()
							estimated, err := landmarkAlgo.LandmarkBasic(output, int32(path.s), int32(path.t))
							if err != nil {
								writeLog(logFile, "ERROR: LandmarkBasic failed: %v", err)
								b.FailNow()
							}
							atomic.AddInt64(&totalEstimatedDistanceTime, time.Since(estimatedStart).Milliseconds())

							atomic.AddInt64(&iterCount, 1)
							atomicAddFloat64(&totalAbsoluteError, math.Abs(float64(estimated-actual)))
						}
					}
				})

				avgActualDistanceTime := float64(totalActualDistanceTime) / float64(iterCount)
				avgEstimatedDistanceTime := float64(totalEstimatedDistanceTime) / float64(iterCount)
				avgError := totalAbsoluteError / float64(iterCount)

				writeLog(logFile, "Avg actual distance time: %.2f ms", avgActualDistanceTime)
				writeLog(logFile, "Avg estimated distance time: %.2f", avgEstimatedDistanceTime)
				writeLog(logFile, "Mean absolute error: %.4f", avgError)

				info, err := os.Stat(output)
				if err != nil {
					writeLog(logFile, "WARN: Failed to get file info for %s: %v", output, err)
				} else {
					writeLog(logFile, "file size: %.2f MB", float64(info.Size())/1024/1024)
				}

				//_ = os.Remove(output)
			}
		}

		logFile.Close()
	}
}

func BenchmarkMainLandmarkShortcut(b *testing.B) {
	graphType := directed
	graphNames := []string{google}
	for _, graphName := range graphNames {
		start := time.Now()
		// Create log file
		logFileName := fmt.Sprintf("%s-landmarkShortcut-bench.txt", graphName)
		logFileName = filepath.Join("output", logFileName)
		logFile, err := os.Create(logFileName)
		if err != nil {
			b.Logf("ERROR: Can't create log file: %v", err)
			b.FailNow()
		}
		input := landmarkAlgo.DatasetInputPath(graphType, fmt.Sprintf("%s-undirected.txt", graphName))
		ugraph, err := graph.FromFile(input, false)
		if err != nil {
			writeLog(logFile, "ERROR: Can't read from %s: %v (took %d ms)", input, err, time.Since(start).Milliseconds())
			b.FailNow()
		}

		nodesSlice := ugraph.GetNodesSlice()

		for _, selectType := range selectTypes {
			selectName := runtime.FuncForPC(reflect.ValueOf(selectType).Pointer()).Name()
			writeLog(logFile, "%s", selectName)
			for _, nodes := range nodesN {
				nodes := nodes
				output := landmarkAlgo.DatasetOutputPath(graphType, fmt.Sprintf("%s-random-path-%d.bin", graphName, nodes))
				dir := filepath.Dir(output)
				if err = os.MkdirAll(dir, os.ModePerm); err != nil {
					writeLog(logFile, "ERROR: Can't create dirs: %v", err)
					b.FailNow()
				}

				used := map[Path]bool{}
				paths := make([]Path, 0, pathNum)

				for len(paths) < pathNum {
					s := nodesSlice[rand.IntN(len(nodesSlice))]
					t := nodesSlice[rand.IntN(len(nodesSlice))]
					for s == t {
						t = nodesSlice[rand.IntN(len(nodesSlice))]
					}
					if s > t {
						s, t = t, s
					}
					path := Path{s, t}
					if _, has := used[path]; has {
						continue
					}
					used[path] = true
					paths = append(paths, path)
				}

				var totalActualDistanceTime int64
				var totalEstimatedDistanceTime int64
				var totalAbsoluteError float64
				var iterCount int64

				b.Run(fmt.Sprintf("bench %s %s landmarks %d", graphName, selectName, nodes), func(b *testing.B) {
					precomputeStart := time.Now()
					writeLog(logFile, "landmarks: %d", nodes)
					err = landmarkAlgo.PrecomputeLandmarksWithPaths(ugraph, output, selectType, nodes)
					if err != nil {
						writeLog(logFile, "ERROR: Precompute failed: %v (took %d ms)", err, time.Since(precomputeStart).Milliseconds())
						b.FailNow()
					}
					writeLog(logFile, "precomputing time: %d ms", time.Since(precomputeStart).Milliseconds())
					for b.Loop() {
						for _, path := range paths {
							actualStart := time.Now()
							actual, err := getAccurateDistance(ugraph, int32(path.s), int32(path.t))
							if err != nil {
								continue
							}
							atomic.AddInt64(&totalActualDistanceTime, time.Since(actualStart).Milliseconds())

							estimatedStart := time.Now()
							estimated, err := landmarkAlgo.LandmarkShortcut(ugraph, output, int32(path.s), int32(path.t))
							if err != nil {
								writeLog(logFile, "ERROR: LandmarkShortcut failed: %v", err)
								b.FailNow()
							}
							atomic.AddInt64(&totalEstimatedDistanceTime, time.Since(estimatedStart).Milliseconds())

							atomic.AddInt64(&iterCount, 1)
							atomicAddFloat64(&totalAbsoluteError, math.Abs(float64(estimated-actual)))
						}
					}
				})

				avgActualDistanceTime := float64(totalActualDistanceTime) / float64(iterCount)
				avgEstimatedDistanceTime := float64(totalEstimatedDistanceTime) / float64(iterCount)
				avgError := totalAbsoluteError / float64(iterCount)

				writeLog(logFile, "Avg actual distance time: %.2f ms", avgActualDistanceTime)
				writeLog(logFile, "Avg estimated distance time: %.2f", avgEstimatedDistanceTime)
				writeLog(logFile, "Mean absolute error: %.4f", avgError)
				info, err := os.Stat(output)
				if err != nil {
					writeLog(logFile, "WARN: Failed to get file info for %s: %v", output, err)
				} else {
					writeLog(logFile, "file size: %.2f MB", float64(info.Size())/1024/1024)
				}
				//_ = os.Remove(output)
			}
		}
		logFile.Close()
	}
}

func writeLog(file *os.File, format string, args ...interface{}) {
	msg := fmt.Sprintf(format, args...)
	_, _ = fmt.Fprintln(file, msg)
}
