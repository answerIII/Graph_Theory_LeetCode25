package landmarkAlgo

import (
	"encoding/binary"
	"errors"
	"fmt"
	"graph_theory/graph"
	"graph_theory/workerpool"
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

	var mu sync.Mutex
	wp := workerpool.NewWorkerPool(runtime.NumCPU(), len(landmarks))
	defer wp.Shutdown()

	for _, u := range landmarks {
		wp.Submit(func() error {
			dists, err := graph.BFS(g, []graph.Node{u}, nil, nil, nil)
			if err != nil {
				return errors.New(fmt.Sprintf("can't calculate distances for node %d\n", u))
			}
			buf := make([]byte, 0, len(nodes))
			for _, v := range nodes {
				var dist int8 = -1
				if d, ok := dists[v]; ok {
					dist = int8(d)
				}
				buf = append(buf, uint8(dist))
			}
			mu.Lock()
			if err = binary.Write(file, binary.LittleEndian, buf); err != nil {
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

	var mu sync.Mutex
	wp := workerpool.NewWorkerPool(runtime.NumCPU(), len(landmarks))
	defer wp.Shutdown()

	for _, u := range landmarks {
		wp.Submit(func() error {
			parents := make(map[graph.Node]graph.Node)
			_, err := graph.BFS(
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

			buf := make([]byte, 0, len(nodes)*4+4)
			buf = binary.LittleEndian.AppendUint32(buf, uint32(u))
			for _, v := range nodes {
				if _, has := parents[v]; !has {
					parents[v] = -1
				}
				buf = binary.LittleEndian.AppendUint32(buf, uint32(parents[v]))
			}
			mu.Lock()
			if err = binary.Write(file, binary.LittleEndian, buf); err != nil {
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
