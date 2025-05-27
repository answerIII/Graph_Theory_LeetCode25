package landmarkAlgo

import (
	"errors"
	"graph_theory/graph"
	"os"
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
		dists, err := graph.BFS(g, []graph.Node{u}, nil, nil, nil)

		if err != nil {
			return errors.New("can't calculate distances for node " + strconv.Itoa(int(u)) + "\n")
		}
		// landmarkFile.WriteString(strconv.Itoa(int(u)) + "\n")
		for _, v := range nodes {
			if value, has := dists[v]; has {
				_, err = landmarkFile.WriteString(strconv.Itoa(value) + "\n")
				if err != nil {
					return err
				}
			} else {
				_, err = landmarkFile.WriteString(strconv.Itoa(-1) + "\n")
				if err != nil {
					return err
				}
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
