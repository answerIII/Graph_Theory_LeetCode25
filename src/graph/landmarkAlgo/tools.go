package landmarkAlgo

import (
	"encoding/binary"
	"errors"
	"fmt"
	"graph_theory/graph"
	"os"
	"path/filepath"
	"runtime"
	"strconv"
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

	for _, u := range landmarks {
		dists, err := graph.BFS(g, []graph.Node{u}, nil, nil, nil)
		if err != nil {
			return errors.New(fmt.Sprintf("can't calculate distances for node %d\n", u))
		}

		for _, v := range nodes {
			var dist int32 = -1
			if d, ok := dists[v]; ok {
				dist = int32(d)
			}
			if err = binary.Write(file, binary.LittleEndian, dist); err != nil {
				return err
			}
		}
	}

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

	landmarkFile, err := os.Create(landmarkFilePath)
	if err != nil {
		return errors.New("can't create landmark file")
	}
	defer landmarkFile.Close()

	_, err = landmarkFile.WriteString(strconv.Itoa(len(g.Nodes)) + "\n")
	if err != nil {
		return err
	}

	nodes := g.GetNodesSlice()

	for _, u := range landmarks {
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
		_, err = landmarkFile.WriteString(strconv.Itoa(int(u)) + "\n")
		if err != nil {
			return err
		}
		for _, v := range nodes {
			if _, has := dists[v]; has {
				node := v
				for node != u {
					_, err = landmarkFile.WriteString(strconv.Itoa(int(node)) + " ")
					if err != nil {
						return err
					}
					node = parents[node]
				}
			} else {
				_, err = landmarkFile.WriteString(strconv.Itoa(-1))
				if err != nil {
					return err
				}
			}
			_, err = landmarkFile.WriteString("\n")
			if err != nil {
				return err
			}
		}
	}

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
