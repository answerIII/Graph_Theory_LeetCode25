package main

import (
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
