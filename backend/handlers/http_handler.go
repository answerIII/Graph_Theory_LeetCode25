package handlers

import (
	"encoding/json"
	"net/http"

	"github.com/HikkMind/graph/middleware"
	"github.com/gorilla/mux"
)

func GetClusteringInfo(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)
	filename := vars["datasetname"]
	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateClustering(middleware.ReadGraph(storageRoot+filename+".msgpack", filename)))

}

func GetDegreeDistribution(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)
	filename := vars["datasetname"]

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateDegrees(middleware.ReadGraph(storageRoot+filename+".msgpack", filename)))
}

func GetDistanceEstimation(w http.ResponseWriter, r *http.Request) {

	type GetDistance struct {
		Method     string `json:"method" msgpack:"method"`
		SampleSize int    `json:"sampleSize" msgpack:"sampleSize"`
	}

	vars := mux.Vars(r)
	filename := vars["datasetname"]

	var req GetDistance
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, "Ошибка разбора JSON", http.StatusBadRequest)
		return
	}

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateDistances(middleware.ReadGraph(storageRoot+filename+".msgpack", filename), req.Method, req.SampleSize))

}

func GetGeneralProperties(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)
	filename := vars["datasetname"]

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateProperties(middleware.ReadGraph(storageRoot+filename+".msgpack", filename)))
}

func GetRobustnessComponent(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)
	filename := vars["datasetname"]

	type GetRobustness struct {
		Percentage []int `json:"xPercentages" msgpack:"xPercentages"`
		// Method     string `json:"removalMethod" msgpack:"removalMethod"`
	}

	var req GetRobustness
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, "Ошибка разбора JSON", http.StatusBadRequest)
		return
	}

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateRobustness(middleware.ReadGraph(storageRoot+filename+".msgpack", filename), req.Percentage))

}

func GetRandomNodes(w http.ResponseWriter, r *http.Request) {
	vars := mux.Vars(r)
	filename := vars["datasetname"]

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateRandomNodes(middleware.ReadGraph(storageRoot+filename+".msgpack", filename)))

}
