package structs

type AnswerA1 struct {
	WCC           Graph
	VertexCount   int
	EdgesCount    int
	Density       float64
	ProportionWCC float32
	WCCCount      int
	ProportionSCC float32
	SCCCount      int
}

type AnswerA2 struct {
	Diameter     int
	Percentile90 float32
}
