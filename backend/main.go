package main

import (
	// "fmt"
	"net/http"
	// "os"
	// "time"

	// //"github.com/HikkMind/graph/algo"

	// "github.com/HikkMind/graph/algo"
	"github.com/HikkMind/graph/handlers"
	// "github.com/HikkMind/graph/middleware"
	// "github.com/vmihailenco/msgpack/v5"
)

func main() {

	http.HandleFunc("/properties", handlers.GetGeneralProperties)

	err := http.ListenAndServe(":8080", nil)
	if err != nil {
		panic(err)
	}

}
