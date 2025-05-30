package main

import (
	"fmt"
	"graph_theory/tools"
	"path/filepath"
	"runtime"
)

func getFilepath(graphType, graphName string) string {
	_, currFile, _, _ := runtime.Caller(0)
	projectRoot := filepath.Join(filepath.Dir(currFile), "..", "..")
	projectRoot = filepath.Join(projectRoot, "datasets")

	filePath := filepath.Clean(projectRoot) + "/" + graphType + "/" + graphName
	return filePath
}

func getLandmarkFilepath(graphName, alg string) (string, error) {
	_, currFile, _, _ := runtime.Caller(0)
	projectRoot := filepath.Join(filepath.Dir(currFile), "..", "..")
	projectRoot = filepath.Join(projectRoot, "graph", "landmarkAlgo", "datasets")

	graphName = tools.GetFileNameWithoutExt(graphName)

	filePath := filepath.Clean(projectRoot) + "/" + graphName
	if alg == "1" {
		filePath += "-random.txt"
	} else if alg == "2" {
		filePath += "-random-paths.txt"
	} else {
		return "", fmt.Errorf("unexpected algorithm: %s", alg)
	}

	return filePath, nil
}
