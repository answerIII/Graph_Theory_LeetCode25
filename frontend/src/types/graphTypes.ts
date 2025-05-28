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
export interface DistanceResultData {
  method: 'double_sweep' | 'random_sample' | 'snowball';
  diameter?: number;
  percentile90?: number;
  meanDistance?: number;
  startNode?: number;
  endNode?: number;
}

// 4. ClusteringComponent
export interface ClusteringData {
  trianglesCount: number;
  globalClusteringCoef: number;
  avgClusteringCoef: number;
  avgClusterCoefLargeWCC: number;
}

// 5. DegreeDistributionComponent
export interface DegreeDistributionData {
  minDegree: number;
  avgDegree: number;
  maxDegree: number;
  probabilityDegree: { [key: number]: number };
}

// 6. RobustnessComponent
export interface RobustnessData {
  xPercent: number;
  removalMethod: 'random' | 'targeted';
  largestComponentFraction: number;
}

// 7. AlgorithmsRuntimeComponent
export interface RuntimeData {
  algorithm: string;
  params: string;
  elapsed_ms: number;
  memory_used_mb: number;
}

// 8. ComparisonComponent
export interface ComparisonData {
  graphId: string;
  properties?: GeneralPropertiesData;
  distances?: DistanceResultData;
  clustering?: ClusteringData;
  degreeDistribution?: DegreeDistributionData;
  robustness?: RobustnessData[];
}

// 9. DistanceAnalysisComponent

export interface DistanceResult {
  id: string;
  algorithm: string;
  distance: number | null;
  execution_time_ms: number;
  landmarks: number[];
  start_node: number;
  end_node: number;
}

export interface AlgorithmParams {
  start_node: string;
  end_node: string;
  landmarks_count: string;
  landmarks_selection: 'random' | 'highest_degree' | 'max_coverage';
}