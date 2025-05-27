package tools

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"slices"
	"strconv"
	"strings"
)

type Nodes []int
type Adjacency map[int][]int
type EdgeFunction func(u, v int) error
type Converter func() (*Nodes, *Adjacency, EdgeFunction)

var SUPPORTED_EXTENSIONS = []string{".csv", ".txt", ".mtx"}

type Parser interface {
	Parse(fileIn io.Reader, f EdgeFunction) (int, error)
}

type CSVParser struct{}

func (p CSVParser) Parse(fileIn io.Reader, f EdgeFunction) (int, error) {
	scanner := bufio.NewScanner(fileIn)

	scanner.Scan()
	numberEdges := 0

	for scanner.Scan() {
		line := scanner.Text()
		parts := strings.Split(line, ",")

		if len(parts) < 2 {
			return 0, fmt.Errorf("invalid line format: %s", line)
		}

		u, err1 := strconv.Atoi(parts[0])
		v, err2 := strconv.Atoi(parts[1])

		if err1 != nil || err2 != nil {
			continue
		}
		if err := f(u, v); err != nil {
			return 0, fmt.Errorf("error while processing file: %w", err)
		}
	}

	if err := scanner.Err(); err != nil {
		return 0, fmt.Errorf("error reading input file: %w", err)
	}

	return numberEdges, nil
}

type TXTParser struct{}

func (p TXTParser) Parse(fileIn io.Reader, f EdgeFunction) (int, error) {
	scanner := bufio.NewScanner(fileIn)

	numberEdges := 0

	for scanner.Scan() {
		line := scanner.Text()
		if strings.HasPrefix(line, "#") || strings.HasPrefix(line, "%") || line == "" {
			continue
		}
		parts := strings.Fields(line)
		if len(parts) < 2 || len(parts) > 2 {
			continue
		}
		numberEdges++
		u, err1 := strconv.Atoi(parts[0])
		v, err2 := strconv.Atoi(parts[1])
		if err1 != nil || err2 != nil {
			continue
		}
		if err := f(u, v); err != nil {
			return 0, fmt.Errorf("error while processing file: %w", err)
		}
	}

	return numberEdges, nil
}

func parser(pathIn string, f EdgeFunction) (int, error) {
	ext := filepath.Ext(pathIn)
	if !slices.Contains(SUPPORTED_EXTENSIONS, ext) {
		return 0, errors.New(ext + " files are unsupported")
	}

	fileIn, err := os.Open(pathIn)
	if err != nil {
		return 0, errors.New("can't open a file " + pathIn)
	}
	defer fileIn.Close()

	var parser Parser

	switch ext {
	case ".txt", ".mtx":
		parser = TXTParser{}
	case ".csv":
		parser = CSVParser{}
	default:
		return 0, errors.New("unsupported file type")
	}

	return parser.Parse(fileIn, f)
}

func removeDuplicate[T comparable](sliceList []T) []T {
	allKeys := make(map[T]bool)
	var list []T
	for _, item := range sliceList {
		if _, value := allKeys[item]; !value {
			allKeys[item] = true
			list = append(list, item)
		}
	}
	return list
}

func convert(pathIn, pathOut string, converter Converter) error {
	ext := filepath.Ext(pathOut)
	if !slices.Contains(SUPPORTED_EXTENSIONS, ext) {
		return errors.New(ext + " files are unsupported")
	}

	fileOut, err := os.Create(pathOut)
	if err != nil {
		return errors.New("can't open a file " + pathOut)
	}
	defer fileOut.Close()

	writer := bufio.NewWriter(fileOut)

	nodes, adj, edgeFunc := converter()

	var numberRealEdges int
	if numberRealEdges, err = parser(pathIn, edgeFunc); err != nil {
		return err
	}

	numberNodes := len(*nodes)
	numberCompressedEdges := 0
	for u := range *nodes {
		numberCompressedEdges += len((*adj)[u])
	}
	_, err = writer.WriteString(strconv.Itoa(numberRealEdges) + " " + strconv.Itoa(numberNodes) + " " + strconv.Itoa(numberCompressedEdges) + "\n")
	if err != nil {
		return errors.New("error while writing a file")
	}

	slices.Sort(*nodes)

	for u := range *nodes {
		(*adj)[u] = removeDuplicate((*adj)[u])
		slices.Sort((*adj)[u])
		for v := range (*adj)[u] {
			_, err := writer.WriteString(strconv.Itoa(int((*nodes)[u])) + " " +
				strconv.Itoa((*adj)[u][v]) + "\n")
			if err != nil {
				return errors.New("error while writing a file")
			}
		}
		if err := writer.Flush(); err != nil {
			return fmt.Errorf("error flushing output buffer: %w", err)
		}
	}
	return nil
}

func SortNodesInFile(pathIn, pathOut string) error {
	err := convert(pathIn, pathOut,
		func() (*Nodes, *Adjacency, EdgeFunction) {
			mapper := map[int]int{}
			nodes := Nodes{}
			adj := Adjacency{}

			mapperIdx := 0

			return &nodes, &adj, func(u, v int) error {
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
				return nil
			}
		})
	if err != nil {
		return err
	}
	return nil
}

func InvertEdgesInFile(pathIn, pathOut string) error {
	err := convert(pathIn, pathOut,
		func() (*Nodes, *Adjacency, EdgeFunction) {
			mapper := map[int]int{}
			nodes := Nodes{}
			adj := Adjacency{}

			mapperIdx := 0

			return &nodes, &adj, func(u, v int) error {
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
				adj[mapper[v]] = append(adj[mapper[v]], mapper[u])
				return nil
			}
		})
	if err != nil {
		return err
	}
	return nil
}

func UndirectEdgesInFile(pathIn, pathOut string) error {
	err := convert(pathIn, pathOut,
		func() (*Nodes, *Adjacency, EdgeFunction) {
			mapper := map[int]int{}
			nodes := Nodes{}
			adj := Adjacency{}

			mapperIdx := 0

			return &nodes, &adj, func(u, v int) error {
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
				adj[mapper[v]] = append(adj[mapper[v]], mapper[u])
				return nil
			}
		})
	if err != nil {
		return err
	}
	return nil
}

func GetFileNameWithoutExt(path string) string {
	base := filepath.Base(path)
	return base[:len(base)-len(filepath.Ext(base))]
}

func GetFileDestination(path string) string {
	return path[:len(path)-len(filepath.Base(path))]
}
