// 1. GraphVisualizationComponent
// export interface GraphData {
//   directed: boolean;
//   nodeCount: number;
//   edges: { from: number; to: number }[];
// }

// 2. GeneralPropertiesComponent
export interface GeneralPropertiesData {
  directed: boolean;
  nodeCount: number;
  edgesCount: number;
  density: number;
  wccCount: number;
  proportionWCC: number;
  sccCount?: number;
  proportionSCC?: number;
}

// 3. DistanceEstimationComponent
export interface DistanceResultEstimation {
  method: 'random_sample' | 'snowball' | 'random_sample_snowball';
  diameter: number;
  percentile90: number;
  meanDistance: number;
  execution_time_ms: number;
}

export interface MethodParams {
  sampleSize: string;
}

export interface DistanceEstimationComponentProps {
  datasetname: string | undefined;
}

// 4. ClusteringComponent
export interface ClusteringData {
  trianglesCount: {
    value: number;
    execution_time_ms: number;
  };
  globalClusteringCoef: {
    value: number;
    execution_time_ms: number;
  };
  avgClusteringCoef: {
    value: number;
    execution_time_ms: number;
  };
  avgClusterCoefLargeWCC: {
    value: number;
    execution_time_ms: number;
  };
}

// 5. DegreeDistributionComponent
export interface DegreeDistributionData {
  minDegree: number;
  avgDegree: number;
  maxDegree: number;
  probabilityDegree: { [key: number]: number };
  execution_time_ms: number;
}

// 6. RobustnessComponent
export interface RobustnessData {
  xPercent: number;
  randomFraction: number;
  targetedFraction: number;
  execution_time_ms: number;
}

// 7. AlgorithmsRuntimeComponent
export interface RuntimeData {
  algorithm: string;
  params: string;
  elapsed_ms: number;
  memory_used_mb: number;
}

// // 8. ComparisonComponent
// export interface ComparisonData {
//   graphId: string;
//   properties?: GeneralPropertiesData;
//   distances?: DistanceResultData;
//   clustering?: ClusteringData;
//   degreeDistribution?: DegreeDistributionData;
//   robustness?: RobustnessData[];
// }

// 9. DistanceAnalysisComponent

export interface DistanceResultAnalysis {
  algorithm: 'bfs' | 'landmarks-basic' | 'landmarks-bfs';
  distance: number | null;
  execution_time_ms: number;
  landmarks: number[];
  start_node: number;
  end_node: number;
}

export interface AlgorithmParams {
  start_node: string;
  end_node: string;
  landmarks_count?: string;
  landmarks_selection?: 'random' | 'highest_degree' | 'max_coverage';
}