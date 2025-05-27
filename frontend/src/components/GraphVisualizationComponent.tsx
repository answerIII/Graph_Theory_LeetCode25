import React, { useEffect, useState } from 'react';
import CytoscapeComponent from 'react-cytoscapejs';
import { Alert, Box, CircularProgress, Typography } from '@mui/material';
import type { Graph } from '../types/interfaces';
import { testGraph1 } from '../constants/graph';

interface GraphVisualizationComponentProps {
  graphId: string;
}

const GraphVisualizationComponent: React.FC<GraphVisualizationComponentProps> = ({ graphId }) => {
  const [graphData, setGraphData] = useState<Graph | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);
  const maxRenderNodes = 1000;

  useEffect(() => {
    // Запрос на бэкенд: GET /graphs/{graphId}
    // const fetchGraph = async () => {
    //   try {
    //     setLoading(true);
    //     const response = await fetch(`/api/graphs/${graphId}`);
    //     if (!response.ok) throw new Error('Ошибка загрузки графа');
    //     const data: Graph = await response.json();
    //     setGraphData(data);
    //   } catch (err) {
    //     setError((err as Error).message);
    //   } finally {
    //     setLoading(false);
    //   }
    // };
    // fetchGraph();
    // Пока используем тестовый граф
    setGraphData(testGraph1);
  }, [graphId]);

  if (loading) return <CircularProgress />;
  if (error) return <Alert severity="error">{error}</Alert>;
  if (!graphData) return null;

  const isLargeGraph = graphData.nodeCount > maxRenderNodes;
  if (isLargeGraph) {
    return <Alert severity="warning">Граф слишком большой для визуализации (более {maxRenderNodes} вершин).</Alert>;
  }

  const elements = [
    ...Array.from({ length: graphData.nodeCount }, (_, i) => ({
      data: { id: i.toString() },
    })),
    ...graphData.edges.map((e, i) => ({
      data: {
        id: `edge-${i}`,
        source: e[0].toString(),
        target: e[1].toString(),
      },
    })),
  ];

  return (
    <Box sx={{ width: '100%', height: '600px' }}>
      <Typography variant="h6" gutterBottom>
        Визуализация графа
      </Typography>
      <CytoscapeComponent
        elements={elements}
        style={{ width: '100%', height: '100%' }}
        layout={{ name: 'cose', animate: true }}
        stylesheet={[
          { selector: 'node', style: { label: 'data(id)', backgroundColor: '#1976d2' } },
          { selector: 'edge', style: { 'curve-style': 'bezier', lineColor: '#ccc' } },
        ]}
      />
    </Box>
  );
};

export default GraphVisualizationComponent;