package handlers

import (
	// "fmt"
	"encoding/json"
	"net/http"

	"github.com/HikkMind/graph/middleware"
	"github.com/gorilla/mux"
)

func GetRobustnessComponent(w http.ResponseWriter, r *http.Request) {
	storageRoot := ""

	vars := mux.Vars(r)
	filename := vars["datasetname"] + ".msgpack"

	type GetRobustness struct {
		Percentage int    `json:"xPercent" msgpack:"xPercent"`
		Method     string `json:"removalMethod" msgpack:"removalMethod"`
	}

	var req GetRobustness
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, "Ошибка разбора JSON", http.StatusBadRequest)
		return
	}

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateRobustness(middleware.ReadGraph(storageRoot+filename), req.Method, req.Percentage))

}
