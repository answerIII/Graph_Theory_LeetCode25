package tools

import (
	"testing"
)

const FILEPATHIN_VK = "../datasets/very_large_graphs/vk.csv"
const FILEPATHOUT_VK = "../datasets/very_large_graphs/vk.txt"

const FILEPATHIN_MUSAE = "../datasets/undirected/musae_git_edges.csv"
const FILEPATHOUT_MUSAE = "../datasets/undirected/musae_git_edges.txt"

func Test_convertCSVtoTxt(t *testing.T) {
	t.Run("Convert vk file", func(t *testing.T) {
		err := convertCSVtoTxt(
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
		err := convertCSVtoTxt(
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
