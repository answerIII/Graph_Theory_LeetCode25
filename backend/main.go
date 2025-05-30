package main

import (
	"fmt"
	"net/http"
	"os"

	"github.com/HikkMind/graph/handlers"
	"github.com/gorilla/mux"
	"github.com/lpernett/godotenv"
)

func main() {
	router := mux.NewRouter()
	err := godotenv.Load("config.env")
	if err != nil {
		panic("failed load env config")
	}
	handlers.InitEnvVars()

	router.HandleFunc("/graphs/{datasetname}/properties", handlers.GetGeneralProperties).Methods("GET")
	router.HandleFunc("/graphs/{datasetname}/distance/estimate", handlers.GetDistanceEstimation).Methods("POST")
	router.HandleFunc("/graphs/{datasetname}/clustering", handlers.GetClusteringInfo).Methods("GET")
	router.HandleFunc("/graphs/{datasetname}/clustering/vertex", handlers.GetClusteringVertex).Methods("GET")
	router.HandleFunc("/graphs/{datasetname}/degree-distribution", handlers.GetDegreeDistribution).Methods("GET")
	router.HandleFunc("/graphs/{datasetname}/robustness", handlers.GetRobustnessComponent).Methods("POST")
	router.HandleFunc("/graphs/{datasetname}/getrandomnodes", handlers.GetRandomNodes).Methods("GET")

	serverPort := os.Getenv("PORT")
	fmt.Println("server started on port " + serverPort)
	err = http.ListenAndServe(serverPort, router)
	if err != nil {
		panic(err)
	}

}
