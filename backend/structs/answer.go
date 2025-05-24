package structs

type AnswerA1 struct {
	WCC           Graph   `json:"-"`
	Directed      bool    `json:"directed"`
	VertexCount   int     `json:"vertexCount"`
	EdgesCount    int     `json:"edgesCount"`
	Density       float64 `json:"density"`
	ProportionWCC float32 `json:"propMaxWeakComponent"`
	WCCCount      int     `json:"weakComponentCount"`
	ProportionSCC float32 `json:"propMaxStrongComponent"`
	SCCCount      int     `json:"strongComponentCount"`
}

type AnswerA2 struct {
	Diameter     int
	Percentile90 float32
}
