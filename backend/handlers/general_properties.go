package handlers

import (
	// "fmt"

	"net/http"

	"github.com/HikkMind/graph/middleware"
	"github.com/gorilla/mux"
)

func GetGeneralProperties(w http.ResponseWriter, r *http.Request) {
	// storageRoot := "../.storage/msgpack/undirected/"
	storageRoot := ""

	vars := mux.Vars(r)
	filename := vars["datasetname"] + ".msgpack"

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateProperties(middleware.ReadGraph(storageRoot + filename)))
	// fmt.Fprintln(w, string(middleware.GenerateA1(middleware.ReadGraph(storageRoot+filename))))
}
