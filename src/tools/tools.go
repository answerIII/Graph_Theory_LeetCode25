package tools

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	"path/filepath"
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
