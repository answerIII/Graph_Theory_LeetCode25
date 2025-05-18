package tools

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"slices"
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

	scanner := bufio.NewScanner(fileIn)
	writer := bufio.NewWriter(fileOut)

	mapper := map[int]int{}
	nodes := []int{}
	adj := map[int][]int{}

	mapperIdx := 0

	for scanner.Scan() {
		line := scanner.Text()
		if strings.HasPrefix(line, "#") || line == "" {
			continue
		}
		parts := strings.Fields(line)
		if len(parts) < 2 {
			continue
		}
		u, err1 := strconv.Atoi(parts[0])
		v, err2 := strconv.Atoi(parts[1])
		if err1 != nil || err2 != nil {
			continue
		}
		if _, has := mapper[u]; !has {
			mapper[u] = mapperIdx
			mapperIdx++
			nodes = append(nodes, mapper[u])
		}
		if _, has := mapper[v]; !has {
			mapper[v] = mapperIdx
			mapperIdx++
			nodes = append(nodes, mapper[v])
		}
		adj[mapper[u]] = append(adj[mapper[u]], mapper[v])
	}

	slices.Sort(nodes)

	for u := range nodes {
		slices.Sort(adj[u])
		for v := range adj[u] {
			_, err := writer.WriteString(strconv.Itoa(int(nodes[u])) + " " +
				strconv.Itoa(int(adj[u][v])) + "\n")
			if err != nil {
				return errors.New("error while writing a file")
			}
		}
		writer.Flush()
	}
	return nil
}
