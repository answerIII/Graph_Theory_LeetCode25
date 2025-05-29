package handlers

import (
	"os"
)

var (
	storageRoot string
)

func InitEnvVars() {
	storageRoot = os.Getenv("STORAGEROOT")
}
