package tools

import (
	"bufio"
	"errors"
	"fmt"
	"graph_theory/graph"
	"os"
	"path/filepath"
	"sort"
	"strconv"
	"strings"
)

func convertCSVtoTxt(pathIn, pathOut string) error {
	if filepath.Ext(pathIn) != ".csv" {
		return errors.New("input file must have a .csv extension")
	}
	if filepath.Ext(pathOut) != ".txt" {
		return errors.New("output file must have a .txt extension")
	}

	fileIn, err := os.Open(pathIn)
	if err != nil {
		return errors.New("input file must have a .csv extension")
	}
	defer fileIn.Close()

	fileOut, err := os.Create(pathOut)
	if err != nil {
		return errors.New("output file must have a .txt extension")
	}
	defer fileOut.Close()

	scanner := bufio.NewScanner(fileIn)
	writer := bufio.NewWriter(fileOut)

	scanner.Scan()

	for scanner.Scan() {
		line := scanner.Text()
		parts := strings.Split(line, ",")

		if len(parts) < 2 {
			return fmt.Errorf("invalid line format: %s", line)
		}

		_, err := writer.WriteString(parts[0] + " " + parts[1] + "\n")
		if err != nil {
			return fmt.Errorf("failed to write to output file: %w", err)
		}
	}

	if err := scanner.Err(); err != nil {
		return fmt.Errorf("error reading input file: %w", err)
	}

	if err := writer.Flush(); err != nil {
		return fmt.Errorf("error flushing output buffer: %w", err)
	}

	return nil
}

func sortNodesInFile(pathIn, pathOut string) error {
	if filepath.Ext(pathIn) != ".txt" {
		return errors.New("input file must have a .txt extension")
	}
	if filepath.Ext(pathOut) != ".txt" {
		return errors.New("output file must have a .txt extension")
	}

	fileIn, err := os.Open(pathIn)
	if err != nil {
		return errors.New("input file must have a .txt extension")
	}
	defer fileIn.Close()

	fileOut, err := os.Create(pathOut)
	if err != nil {
		return errors.New("output file must have a .txt extension")
	}
	defer fileOut.Close()

	writer := bufio.NewWriter(fileOut)

	g, err := graph.FromFile(pathIn, true)

	nodes := make([]graph.Node, 0, len(g.Nodes))
	for k := range g.Nodes {
		nodes = append(nodes, k)
	}

	sort.Slice(nodes, func(i, j int) bool { return nodes[i] < nodes[j] })

	for u := range nodes {
		adj := make([]graph.Node, 0, len(g.Adj[nodes[u]]))
		for v := range g.Adj[nodes[u]] {
			adj = append(adj, v)
		}
		sort.Slice(adj, func(i, j int) bool { return adj[i] < adj[j] })

		for v := range adj {
			_, err := writer.WriteString(strconv.Itoa(int(nodes[u])) + " " + strconv.Itoa(int(adj[v])) + "\n")
			if err != nil {
				return errors.New("error while writing a file")
			}
		}
		writer.Flush()
	}
	return nil
}
