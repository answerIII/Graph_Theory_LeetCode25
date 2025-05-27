package main

import (
	// "fmt"
	"net/http"
	// "os"
	// "time"

	// //"github.com/HikkMind/graph/algo"

	// "github.com/HikkMind/graph/algo"
	"github.com/HikkMind/graph/handlers"
	"github.com/gorilla/mux"
	// "github.com/HikkMind/graph/middleware"
	// "github.com/vmihailenco/msgpack/v5"
)

func main() {
	router := mux.NewRouter()

	// Регистрируем маршрут
	router.HandleFunc("/graphs/{datasetname}/properties", handlers.GetGeneralProperties).Methods("GET")

	err := http.ListenAndServe(":8080", router)
	if err != nil {
		panic(err)
	}
}
