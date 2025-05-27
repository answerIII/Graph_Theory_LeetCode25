package main

import (
	// "fmt"
	"fmt"
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

	router.HandleFunc("/graphs/{datasetname}/properties", handlers.GetGeneralProperties).Methods("GET")
	router.HandleFunc("/graphs/{datasetname}/distances", handlers.GetDistanceEstimation).Methods("GET")
	router.HandleFunc("/graphs/{datasetname}/clustering", handlers.GetClusteringInfo).Methods("GET")
	router.HandleFunc("/graphs/{datasetname}/degree-distribution", handlers.GetDegreeDistribution).Methods("GET")
	router.HandleFunc("/graphs/{datasetname}/robustness", handlers.GetRobustnessComponent).Methods("GET")

	serverPort := ":8080"
	fmt.Println("server started on port " + serverPort)
	err := http.ListenAndServe(serverPort, router)
	if err != nil {
		panic(err)
	}

	// stdFileName := "vk.msgpack"
	// stdFileName := "test.json"
	// // stdStorageName := "../.storage/msgpack/very_large_graphs"
	// stdStorageName := "../.storage"

	// if len(os.Args) > 1 {
	// 	stdFileName = os.Args[1]
	// 	stdStorageName = ""
	// }

	// start := time.Now()
	// graph := middleware.ReadGraph(stdStorageName + "/" + stdFileName)
	// // fmt.Println(graph)
	// fmt.Println(graph.VertexCount, graph.EdgesCount)
	// fmt.Println(time.Since(start))
	// fmt.Printf("got graph\n\n")

	// start = time.Now()
	// fmt.Println(algo.FindDiameter(&graph))
	// fmt.Println(time.Since(start))

	// fmt.Println(algo.GetRandomVertexSet(&graph, 30))
	// start := time.Now()
	// fmt.Println(algo.CountTriangles(&graph))
	// graphA1, _ := algo.FindMaxWCC(&graph)
	// fmt.Println(algo.AvgClusterCoef(&graph))
	// fmt.Println(algo.GetDegreeProbability(&graph))
	// fmt.Println(algo.SnowBall(&graph, 4))
	// fmt.Println(time.Since(start))

	// s, _ := json.Marshal(graphA1)
	// s := middleware.GenerateA1(&graph)
	// fmt.Println(string(s))
	// fmt.Println(graph)

}
