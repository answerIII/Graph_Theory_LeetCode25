package handlers

import (
	// "fmt"
	"net/http"

	"github.com/HikkMind/graph/middleware"
	"github.com/gorilla/mux"
)

func GetGeneralProperties(w http.ResponseWriter, r *http.Request) {
	storageRoot := "./"

	vars := mux.Vars(r)
	filename := vars["datasetname"] + ".msgpack"

	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateA1(middleware.ReadGraph(storageRoot + filename)))
	// fmt.Fprintln(w, string(middleware.GenerateA1(middleware.ReadGraph("E:\\Programming\\uni\\graphs\\project\\Graph_Theory_LeetCode25\\.storage\\test.json"))))
}
