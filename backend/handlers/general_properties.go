package handlers

import (
	// "fmt"
	"fmt"
	"net/http"

	"github.com/HikkMind/graph/middleware"
	"github.com/gorilla/mux"
)

func GetGeneralProperties(w http.ResponseWriter, r *http.Request) {
	// storageRoot := "../.storage/msgpack/undirected/"
	storageRoot := "E:\\Programming\\uni\\graphs\\project\\Graph_Theory_LeetCode25\\.storage\\msgpack\\undirected\\"

	vars := mux.Vars(r)
	filename := vars["datasetname"] + ".msgpack"

	w.WriteHeader(http.StatusOK)
	// w.Write(middleware.GenerateA1(middleware.ReadGraph(storageRoot + filename)))
	fmt.Fprintln(w, string(middleware.GenerateA1(middleware.ReadGraph(storageRoot+filename))))
}
