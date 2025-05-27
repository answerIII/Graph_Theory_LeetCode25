import React, { useState } from 'react';
import { Table, TableBody, TableCell, TableHead, TableRow, Button } from '@mui/material';

interface GraphMetrics {
  graphId: string;
  properties: GeneralProperties;
  distances: DistanceResult;
  clustering: ClusteringData;
  degrees: DegreeData[];
  robustness: RobustnessResult[];
}

const ComparisonComponent: React.FC = () => {
  const [graphs, setGraphs] = useState<GraphMetrics[]>([]);

  const addGraph = async (graphId: string) => {
    try {
      const [properties, distances, clustering, degrees, robustness] = await Promise.all([
        fetch(`/api/graphs/${graphId}/properties`).then((res) => res.json()),
        fetch(`/api/graphs/${graphId}/distances`, { method: 'POST', body: JSON.stringify({ method: 'double_sweep' }) }).then((res) => res.json()),
        fetch(`/api/graphs/${graphId}/clustering`).then((res) => res.json()),
        fetch(`/api/graphs/${graphId}/degree-distribution`).then((res) => res.json()),
        fetch(`/api/graphs/${graphId}/robustness`, { method: 'POST', body: JSON.stringify({ xPercent: 10, removalMethod: 'random' }) }).then((res) => res.json()),
      ]);
      setGraphs([...graphs, { graphId, properties, distances, clustering, degrees, robustness }]);
    } catch (err) {
      console.error(err);
    }
  };

  return (
    <div>
      <Button onClick={() => addGraph('test_graph')}>Добавить тестовый граф</Button>
      <Table>
        <TableHead>
          <TableRow>
            <TableCell>Граф</TableCell>
            <TableCell>Вершины</TableCell>
            <TableCell>Плотность</TableCell>
            <TableCell>Диаметр</TableCell>
            <TableCell>Кластеризация</TableCell>
            <TableCell>Устойчивость (10%)</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {graphs.map((g) => (
            <TableRow key={g.graphId}>
              <TableCell>{g.graphId}</TableCell>
              <TableCell>{g.properties.vertexCount}</TableCell>
              <TableCell>{g.properties.density.toFixed(4)}</TableCell>
              <TableCell>{g.distances.diameter}</TableCell>
              <TableCell>{g.clustering.avgClusteringCoef.toFixed(4)}</TableCell>
              <TableCell>{g.robustness[0]?.largestComponentFraction.toFixed(4)}</TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </div>
  );
};

export default ComparisonComponent;