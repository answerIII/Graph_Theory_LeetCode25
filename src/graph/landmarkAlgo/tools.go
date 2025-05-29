package landmarkAlgo

import (
	"encoding/binary"
	"errors"
	"fmt"
	"graph_theory/graph"
	"graph_theory/workerpool"
	"log"
	"os"
	"path/filepath"
	"runtime"
	"strconv"
	"sync"
)

func PrecomputeLandmarks(
	g *graph.Graph,
	landmarkFilePath string,
	selectFunction func(*graph.Graph, int) ([]graph.Node, error),
	nodesN int,
) error {
	log.Println("Selecting")
	landmarks, err := selectFunction(g, nodesN)
	if err != nil {
		return errors.New("can't select nodes for landmarks")
	}

	file, err := os.Create(landmarkFilePath)
	if err != nil {
		return errors.New("can't create landmark file")
	}
	defer file.Close()

	header := []int32{int32(len(g.Nodes)), int32(nodesN)}
	if err = binary.Write(file, binary.LittleEndian, header); err != nil {
		return err
	}

	nodes := g.GetNodesSlice()

	log.Println("Calculating distances")

	var mu sync.Mutex
	wp := workerpool.NewWorkerPool(runtime.NumCPU(), len(landmarks))
	defer wp.Shutdown()

	for _, u := range landmarks {
		wp.Submit(func() error {
			dists, err := graph.BFS(g, []graph.Node{u}, nil, nil, nil)
			if err != nil {
				return errors.New(fmt.Sprintf("can't calculate distances for node %d\n", u))
			}
			realBuf := make([]byte, len(nodes)*4)
			buf := realBuf
			for _, v := range nodes {
				var dist int32 = -1
				if d, ok := dists[v]; ok {
					dist = int32(d)
					binary.LittleEndian.PutUint32(buf, uint32(dist))
					buf = buf[4:]
				}
			}
			mu.Lock()
			if err = binary.Write(file, binary.LittleEndian, realBuf); err != nil {
				return err
			}
			mu.Unlock()
			return nil
		})
	}

	wp.Wait()

	return nil
}

func PrecomputeLandmarksWithPaths(
	g *graph.Graph,
	landmarkFilePath string,
	selectFunction func(*graph.Graph, int) ([]graph.Node, error),
	nodesN int,
) error {
	log.Println("Selecting")
	landmarks, err := selectFunction(g, nodesN)
	if err != nil {
		return errors.New("can't select nodes for landmarks")
	}

	file, err := os.Create(landmarkFilePath)
	if err != nil {
		return errors.New("can't create landmark file")
	}
	defer file.Close()

	header := []int32{int32(len(g.Nodes)), int32(nodesN)}
	if err = binary.Write(file, binary.LittleEndian, header); err != nil {
		return err
	}

	log.Println("Calculating distances")
	nodes := g.GetNodesSlice()

	var mu sync.Mutex
	wp := workerpool.NewWorkerPool(runtime.NumCPU(), len(landmarks))
	defer wp.Shutdown()

	for _, u := range landmarks {
		wp.Submit(func() error {
			parents := make(map[graph.Node]graph.Node)
			dists, err := graph.BFS(
				g,
				[]graph.Node{u},
				nil,
				func(node, parent graph.Node, dist int) {
					parents[node] = parent
				},
				nil)

			if err != nil {
				return errors.New("can't calculate distances for node " + strconv.Itoa(int(u)) + "\n")
			}

			cnt := 0
			paths := make([][]int32, 0)

			for _, v := range nodes {
				if _, has := dists[v]; has {
					path := make([]int32, 0)
					node := v
					for node != u {
						path = append(path, int32(node))
						node = parents[node]
					}
					paths = append(paths, path)
					cnt += len(path)
				} else {
					paths = append(paths, []int32{-1})
					cnt += 1
				}
			}

			totalSize := cnt*4 + len(nodes)*2 + 4
			realBuf := make([]byte, totalSize)
			buf := realBuf
			binary.LittleEndian.PutUint32(buf, uint32(u))
			buf = buf[4:]

			for _, path := range paths {
				binary.LittleEndian.PutUint16(buf, uint16(len(path)))
				buf = buf[2:]
				for _, v := range path {
					binary.LittleEndian.PutUint32(buf, uint32(v))
					buf = buf[4:]
				}
			}
			mu.Lock()
			if err = binary.Write(file, binary.LittleEndian, realBuf); err != nil {
				return err
			}
			mu.Unlock()
			return nil
		})
	}

	wp.Wait()

	return nil
}

func ProjectRoot() string {
	_, currFile, _, _ := runtime.Caller(0)
	projectRoot := filepath.Join(filepath.Dir(currFile), "..", "..")
	return filepath.Clean(projectRoot)
}

func DatasetInputPath(parts ...string) string {
	if len(parts) < 2 {
		panic("DatasetInputPath requires at least 2 parts: [subdir, filename...]")
	}

	pathParts := []string{ProjectRoot(), "datasets", parts[0], "aux_graphs"}
	pathParts = append(pathParts, parts[1:]...)
	return filepath.Join(pathParts...)
}

func DatasetOutputPath(parts ...string) string {
	return filepath.Join(append([]string{ProjectRoot(), "graph", "landmarkAlgo", "datasets"}, parts...)...)
}
