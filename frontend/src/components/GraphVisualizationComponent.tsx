import React, { useEffect, useState } from 'react';
import CytoscapeComponent from 'react-cytoscapejs';
import { Alert, Box, Typography } from '@mui/material';
import type { Graph } from '../types/interfaces';

interface GraphVisualizationComponentProps {
  graphId: string;
}

const GraphVisualizationComponent: React.FC<GraphVisualizationComponentProps> = ({ graphId }) => {
  const [graph, setGraph] = useState<Graph | null>(null);
  const maxRenderNodes = 1000;

  useEffect(() => {
    // Запрос на бэкенд: GET /graphs/{graphId}
    // Пока используем тестовый граф
    setGraph({
      directed: false,
      numNodes: 5,
      edges: [
        [0, 1],
        [1, 2],
        [2, 3],
        [3, 4],
        [4, 0],
      ],
    });
  }, [graphId]);

  if (!graph) return <Typography>Загрузка...</Typography>;

  const isLargeGraph = graph.numNodes > maxRenderNodes;
  if (isLargeGraph) {
    return <Alert severity="warning">Граф слишком большой для визуализации (более {maxRenderNodes} вершин).</Alert>;
  }

  const elements = [
    ...Array.from({ length: graph.numNodes }, (_, i) => ({
      data: { id: i.toString() },
    })),
    ...graph.edges.map((e, i) => ({
      data: {
        id: `edge-${i}`,
        source: e[0].toString(),
        target: e[1].toString(),
      },
    })),
  ];

  return (
    <Box>
      <Typography variant="h6" gutterBottom>
        Визуализация графа
      </Typography>
      <CytoscapeComponent
        elements={elements}
        style={{ width: '100%', height: '400px' }}
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