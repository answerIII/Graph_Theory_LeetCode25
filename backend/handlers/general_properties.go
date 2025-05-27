package handlers

import (
	// "fmt"
	"net/http"

	"github.com/HikkMind/graph/middleware"
)

func GetGeneralProperties(w http.ResponseWriter, r *http.Request) {
	w.WriteHeader(http.StatusOK)
	w.Write(middleware.GenerateA1(middleware.ReadGraph("E:\\Programming\\uni\\graphs\\project\\Graph_Theory_LeetCode25\\.storage\\test.json")))
	// fmt.Fprintln(w, string(middleware.GenerateA1(middleware.ReadGraph("E:\\Programming\\uni\\graphs\\project\\Graph_Theory_LeetCode25\\.storage\\test.json"))))
}
