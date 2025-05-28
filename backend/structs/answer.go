package structs

type AnswerA1 struct {
	WCC           *Graph  `json:"-" msgpack:"-"`
	Directed      bool    `json:"directed" msgpack:"directed"`
	VertexCount   int     `json:"nodeCount" msgpack:"nodeCount"`
	EdgesCount    int     `json:"edgesCount" msgpack:"edgesCount"`
	Density       float64 `json:"density" msgpack:"density"`
	WCCCount      int     `json:"wccCount" msgpack:"wccCount"`
	ProportionWCC float32 `json:"proportionWCC" msgpack:"proportionWCC"`
	SCCCount      int     `json:"sccCount" msgpack:"sccCount"`
	ProportionSCC float32 `json:"proportionSCC" msgpack:"proportionSCC"`
}

type AnswerA2 struct {
	Diameter     int
	Percentile90 int
	MeanDistance float32
}

type AnswerA3 struct {
	TrianglesCount    int `json:"trianglesCount"`
	GlobalClusterCoef float64 `json:"globalClusteringCoef"`
	AvgClusterCoef    float64 `json:"avgClusteringCoef"`
	AvgClusterCoefWCC float64 `json:"avgClusterCoefLargeWCC"`
}

type AnswerA5 struct {
	MinDegree         int
	AvgDegree         float64
	MaxDegree         int
	ProbabilityDegree map[int]float64
}
