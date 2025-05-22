package graph

import "math"

const (
	exampleFilepath         = "../datasets/directed/aux_graphs/example-sorted.txt"
	socWikiVoteFilepath     = "../datasets/directed/aux_graphs/soc-wiki-Vote-sorted.txt"
	webGoogleFilepath       = "../datasets/directed/aux_graphs/web-Google-sorted.txt"
	webNotreDameFilepath    = "../datasets/directed/aux_graphs/web-NotreDame-sorted.txt"
	webStanfordFilepath     = "../datasets/directed/aux_graphs/web-Stanford-sorted.txt"
	wikiVoteFilepath        = "../datasets/directed/aux_graphs/Wiki-Vote-sorted.txt"
	caAstroPhFilepath       = "../datasets/undirected/aux_graphs/CA-AstroPh-sorted.txt"
	caCoauthorsDblpFilepath = "../datasets/undirected/aux_graphs/ca-coauthors-dblp-sorted.txt"
	caGrqcFilepath          = "../datasets/undirected/aux_graphs/CA-GrQc-sorted.txt"
	emailEuAllFilepath      = "../datasets/undirected/aux_graphs/Email-EuAll-sorted.txt"
	musaeGitEdgesFilepath   = "../datasets/undirected/aux_graphs/musae-git-edges-sorted.csv"
	youtubeUngraphFilepath  = "../datasets/very_large_graphs/aux_graphs/com-youtube.ungraph-sorted.txt"
	vkFilepath              = "../datasets/very_large_graphs/aux_graphs/vk-sorted.txt"
	orkkutFilepath          = "../datasets/very_large_graphs/aux_graphs/com-orkut.ungraph-sorted.txt"

	ugraphExampleFilepath         = "../datasets/directed/aux_graphs/example-undirected.txt"
	ugraphSocWikiVoteFilepath     = "../datasets/directed/aux_graphs/soc-wiki-Vote-undirected.txt"
	ugraphWebGoogleFilepath       = "../datasets/directed/aux_graphs/web-Google-undirected.txt"
	ugraphWebNotreDameFilepath    = "../datasets/directed/aux_graphs/web-NotreDame-undirected.txt"
	ugraphWebStanfordFilepath     = "../datasets/directed/aux_graphs/web-Stanford-undirected.txt"
	ugraphWikiVoteFilepath        = "../datasets/directed/aux_graphs/Wiki-Vote-undirected.txt"
	ugraphCaAstroPhFilepath       = "../datasets/undirected/aux_graphs/CA-AstroPh-undirected.txt"
	ugraphCaCoauthorsDblpFilepath = "../datasets/undirected/aux_graphs/ca-coauthors-dblp-undirected.txt"
	ugraphCaGrqcFilepath          = "../datasets/undirected/aux_graphs/CA-GrQc-undirected.txt"
	ugraphEmailEuAllFilepath      = "../datasets/undirected/aux_graphs/Email-EuAll-undirected.txt"
	ugraphMusaeGitEdgesFilepath   = "../datasets/undirected/aux_graphs/musae_git_edges-undirected.txt"
	ugraphYoutubeUngraphFilepath  = "../datasets/very_large_graphs/aux_graphs/com-youtube-ungraph-undirected.txt"
	ugraphVkFilepath              = "../datasets/very_large_graphs/aux_graphs/vk-undirected.txt"
	ugraphOrkkutFilepath          = "../datasets/very_large_graphs/aux_graphs/com-orkut.ungraph-undirected.txt"

	invertedExampleFilepath         = "../datasets/directed/aux_graphs/example-inverted.txt"
	invertedSocWikiVoteFilepath     = "../datasets/directed/aux_graphs/soc-wiki-Vote-inverted.txt"
	invertedWebGoogleFilepath       = "../datasets/directed/aux_graphs/web-Google-inverted.txt"
	invertedWebNotreDameFilepath    = "../datasets/directed/aux_graphs/web-NotreDame-inverted.txt"
	invertedWebStanfordFilepath     = "../datasets/directed/aux_graphs/web-Stanford-inverted.txt"
	invertedWikiVoteFilepath        = "../datasets/directed/aux_graphs/Wiki-Vote-inverted.txt"
	invertedCaAstroPhFilepath       = "../datasets/undirected/aux_graphs/CA-AstroPh-inverted.txt"
	invertedCaCoauthorsDblpFilepath = "../datasets/undirected/aux_graphs/ca-coauthors-dblp-inverted.txt"
	invertedCaGrqcFilepath          = "../datasets/undirected/aux_graphs/CA-GrQc-inverted.txt"
	invertedEmailEuAllFilepath      = "../datasets/undirected/aux_graphs/Email-EuAll-inverted.txt"
	invertedMusaeGitEdgesFilepath   = "../datasets/undirected/aux_graphs/musae-git-edges-inverted.txt"
	invertedYoutubeUngraphFilepath  = "../datasets/very_large_graphs/aux_graphs/com-youtube-ungraph-inverted.txt"
	invertedVkFilepath              = "../datasets/very_large_graphs/aux_graphs/vk-inverted.txt"
	invertedOrkkutFilepath          = "../datasets/very_large_graphs/aux_graphs/com-orkut.ungraph-inverted.txt"
)

func roundFloat(val float64, precision uint) float64 {
	ratio := math.Pow(10, float64(precision))
	return math.Round(val*ratio) / ratio
}
