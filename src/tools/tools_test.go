package tools

import (
	"testing"
)

const (
	exampleFilepath         = "../datasets/directed/example.txt"
	socWikiVoteFilepath     = "../datasets/directed/soc-wiki-Vote.mtx"
	webGoogleFilepath       = "../datasets/directed/web-Google.txt"
	webNotreDameFilepath    = "../datasets/directed/web-NotreDame.txt"
	webStanfordFilepath     = "../datasets/directed/web-Stanford.txt"
	wikiVoteFilepath        = "../datasets/directed/Wiki-Vote.txt"
	caAstroPhFilepath       = "../datasets/undirected/CA-AstroPh.txt"
	caCoauthorsDblpFilepath = "../datasets/undirected/ca-coauthors-dblp.txt"
	caGrqcFilepath          = "../datasets/undirected/CA-GrQc.txt"
	emailEuAllFilepath      = "../datasets/undirected/Email-EuAll.txt"
	musaeGitEdgesFilepath   = "../datasets/undirected/musae_git_edges.csv"
	youtubeUngraphFilepath  = "../datasets/very_large_graphs/com-youtube.ungraph.txt"
	vkFilepath              = "../datasets/very_large_graphs/vk.csv"
	output                  = ".output.txt"
)

func Test_parseCsvFiles(t *testing.T) {
	t.Run("Parse vk file", func(t *testing.T) {
		if err := parser(vkFilepath, func(u, v int) error { return nil }); err != nil {
			t.Errorf("Error parsing CSV: %v", err)
		} else {
			t.Logf("Successfully parsed %s", vkFilepath)
		}
	})

	t.Run("Parse musae file", func(t *testing.T) {
		if err := parser(musaeGitEdgesFilepath, func(u, v int) error { return nil }); err != nil {
			t.Errorf("Error parsing CSV: %v", err)
		} else {
			t.Logf("Successfully parsed %s", musaeGitEdgesFilepath)
		}
	})
}

func Test_sortNodesInFile(t *testing.T) {
	t.Run("Sort Wiki-Vote file", func(t *testing.T) {
		err := SortNodesInFile(wikiVoteFilepath, output)
		if err != nil {
			t.Errorf("Error sorting a file %v", err)
		} else {
			t.Logf("Successfully sorted %s to %s", wikiVoteFilepath, output)
		}
	})
}

func Test_invertEdgesInFile(t *testing.T) {
	t.Run("Invert example file", func(t *testing.T) {
		err := InvertEdgesInFile(exampleFilepath, output)
		if err != nil {
			t.Errorf("Error inverting edges in file %v", err)
		} else {
			t.Logf("Successfully invert edges from %s, to %s", exampleFilepath, output)
		}
	})
}

func Test_undirectEdgesInFile(t *testing.T) {
	t.Run("Undirect example file", func(t *testing.T) {
		err := UndirectEdgesInFile(exampleFilepath, output)
		if err != nil {
			t.Errorf("Error inverting edges in file %v", err)
		} else {
			t.Logf("Successfully invert edges from %s, to %s", exampleFilepath, output)
		}
	})
}
