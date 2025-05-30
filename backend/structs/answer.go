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
	TimeWCCms     int     `json:"timeWCCMs" msgpack:"timeWCCMs"`
	TimeSCCms     int     `json:"timeSCCMs" msgpack:"timeSCCMs"`
}

type AnswerA2 struct {
	Diameter     int     `json:"diameter" msgpack:"diameter"`
	Percentile90 int     `json:"percentile90" msgpack:"percentile90"`
	MeanDistance float32 `json:"meanDistance" msgpack:"meanDistance"`
	TimeMs       int     `json:"execution_time_ms" msgpack:"execution_time_ms"`
}

type AnswerA3 struct {
	TrianglesCount    ValueTime `json:"trianglesCount" msgpack:"trianglesCount"`
	GlobalClusterCoef ValueTime `json:"globalClusteringCoef" msgpack:"globalClusteringCoef"`
	AvgClusterCoef    ValueTime `json:"avgClusteringCoef" msgpack:"avgClusteringCoef"`
	AvgClusterCoefWCC ValueTime `json:"avgClusterCoefLargeWCC" msgpack:"avgClusterCoefLargeWCC"`
	// TimeTrianglesCountMs    int     `json:"timeTrianglesMs" msgpack:"timeTrianglesMs"`
	// TimeAvgClusterCoefMs    int     `json:"timeAvgClusterMs" msgpack:"timeAvgClusterMs"`
	// TimeAvgClusterCoefWCCMs int     `json:"timeAvgClusterWCCMs" msgpack:"timeAvgClusterWCCMs"`
}

type AnswerA5 struct {
	MinDegree         int             `json:"minDegree" msgpack:"minDegree"`
	AvgDegree         float64         `json:"avgDegree" msgpack:"avgDegree"`
	MaxDegree         int             `json:"maxDegree" msgpack:"maxDegree"`
	ProbabilityDegree map[int]float64 `json:"probabilityDegree" msgpack:"probabilityDegree"`
	TimeMs            int             `json:"execution_time_ms" msgpack:"execution_time_ms"`
}

// type AnswerB struct {
// 	Percentage    int             `json:"xPercent"`
// 	Method        string          `json:"removalMethod"`
// 	ProportionWCC map[int]float32 `json:"largestComponentFraction"`
// 	TimeMs        int             `json:"execution_time_ms" msgpack:"execution_time_ms"`
// }

type AnswerB struct {
	Percentage     int     `json:"xPercent" msgpack:"xPercent"`
	RandomFraction float32 `json:"randomFraction" msgpack:"randomFraction"`
	TargetFraction float32 `json:"targetedFraction" msgpack:"targetedFraction"`
	TimeMs         int     `json:"execution_time_ms" msgpack:"execution_time_ms"`
}
