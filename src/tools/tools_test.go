package tools

import (
	"testing"
)

const FILEPATHIN_VK = "../datasets/very_large_graphs/vk.csv"
const FILEPATHOUT_VK = "../datasets/very_large_graphs/vk.txt"

const FILEPATHIN_MUSAE = "../datasets/undirected/musae_git_edges.csv"
const FILEPATHOUT_MUSAE = "../datasets/undirected/musae_git_edges.txt"

const FILEPATHIN_WIKI = "../datasets/directed/Wiki-Vote.txt"
const FILEPATHOUT_WIKI = "../datasets/directed/Wiki-Vote-sorted.txt"

const FILEPATHIN_EXAMPLE = "../datasets/directed/example.txt"
const FILEPATHOUT_EXAMPLE = "../datasets/directed/example-inverted.txt"

func Test_convertCSVtoTxt(t *testing.T) {
	t.Run("Convert vk file", func(t *testing.T) {
		err := sortNodesInFile(
			FILEPATHIN_VK,
			FILEPATHOUT_VK,
		)
		if err != nil {
			t.Errorf("Error converting CSV to TXT: %v", err)
		} else {
			t.Logf("Successfully converted %s to %s", FILEPATHIN_VK, FILEPATHOUT_VK)
		}
	})

	t.Run("Convert musae file", func(t *testing.T) {
		err := sortNodesInFile(
			FILEPATHIN_MUSAE,
			FILEPATHOUT_MUSAE,
		)
		if err != nil {
			t.Errorf("Error converting CSV to TXT: %v", err)
		} else {
			t.Logf("Successfully converted %s to %s", FILEPATHIN_MUSAE, FILEPATHOUT_MUSAE)
		}
	})
}

func Test_sortNodesInFile(t *testing.T) {
	t.Run("Sort Wiki-Vote file", func(t *testing.T) {
		err := sortNodesInFile(FILEPATHIN_WIKI, FILEPATHOUT_WIKI)
		if err != nil {
			t.Errorf("Error sorting a file %v", err)
		} else {
			t.Logf("Successfully sorted %s to %s", FILEPATHIN_WIKI, FILEPATHOUT_WIKI)
		}
	})
}

func Test_invertEdgesInFile(t *testing.T) {
	t.Run("Invert example file", func(t *testing.T) {
		err := invertEdgesInFile(FILEPATHIN_EXAMPLE, FILEPATHOUT_EXAMPLE)
		if err != nil {
			t.Errorf("Error inverting edges in file %v", err)
		} else {
			t.Logf("Successfully invert edges from %s, to %s", FILEPATHIN_EXAMPLE, FILEPATHOUT_EXAMPLE)
		}
	})
}

func Test_undirectEdgesInFile(t *testing.T) {
	t.Run("Undirect example file", func(t *testing.T) {
		err := undirectEdgesInFile(FILEPATHIN_EXAMPLE, FILEPATHOUT_EXAMPLE)
		if err != nil {
			t.Errorf("Error inverting edges in file %v", err)
		} else {
			t.Logf("Successfully invert edges from %s, to %s", FILEPATHIN_EXAMPLE, FILEPATHOUT_EXAMPLE)
		}
	})
}
