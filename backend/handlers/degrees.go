package handlers

import (
	// "fmt"

	"net/http"

	"github.com/HikkMind/graph/middleware"
	"github.com/gorilla/mux"
)

func GetDegreeDistribution(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)
	filename := vars["datasetname"] + ".msgpack"

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateDegrees(middleware.ReadGraph(storageRoot + filename)))
}
