package handlers

import (
	// "fmt"

	"net/http"

	"github.com/HikkMind/graph/middleware"
	"github.com/gorilla/mux"
)

func GetClusteringInfo(w http.ResponseWriter, r *http.Request) {
	storageRoot := ""

	vars := mux.Vars(r)
	filename := vars["datasetname"] + ".msgpack"

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateClustering(middleware.ReadGraph(storageRoot + filename)))

}
