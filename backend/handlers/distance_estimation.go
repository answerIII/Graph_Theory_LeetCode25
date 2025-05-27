package handlers

import (
	// "fmt"
	"encoding/json"

	"net/http"

	"github.com/HikkMind/graph/middleware"
	"github.com/gorilla/mux"
)

func GetDistanceEstimation(w http.ResponseWriter, r *http.Request) {

	type GetDistance struct {
		Method     string `json:"method" msgpack:"method"`
		SampleSize int    `json:"sampleSize" msgpack:"sampleSize"`
	}

	// storageRoot := "../.storage/msgpack/undirected/"
	storageRoot := ""

	vars := mux.Vars(r)
	filename := vars["datasetname"] + ".msgpack"

	var req GetDistance
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, "Ошибка разбора JSON", http.StatusBadRequest)
		return
	}

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateA2(middleware.ReadGraph(storageRoot+filename), req.Method, req.SampleSize))

}
