import React, { useState, useCallback, Component, type ReactNode } from 'react';
import {
  Box,
  Button,
  TextField,
  Typography,
  Card,
  CardContent,
  Alert,
  RadioGroup,
  FormControlLabel,
  Radio,
  FormControl,
  FormLabel,
  LinearProgress,
} from '@mui/material';
import CytoscapeComponent from 'react-cytoscapejs';
import type { Graph } from '../types/graph';

interface GraphAnalyzerProps {
  graph: Graph;
}

// Error Boundary для обработки ошибок визуализации
class ErrorBoundary extends Component<{ children: ReactNode }, { hasError: boolean }> {
  state = { hasError: false };

  static getDerivedStateFromError() {
    return { hasError: true };
  }

  render() {
    if (this.state.hasError) {
      return <Alert severity="error">Ошибка визуализации графа</Alert>;
    }
    return this.props.children;
  }
}

const GraphAnalyzer: React.FC<GraphAnalyzerProps> = ({ graph }) => {
  const [fromVertex, setFromVertex] = useState('');
  const [toVertex, setToVertex] = useState('');
  const [distance, setDistance] = useState<number | null>(null);
  const [clusterVertex, setClusterVertex] = useState('');
  const [clusterCoeff, setClusterCoeff] = useState<number | null>(null);
  const [removePercent, setRemovePercent] = useState('');
  const [removeStrategy, setRemoveStrategy] = useState<'random' | 'degree'>('random');
  const [componentFraction, setComponentFraction] = useState<number | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [computing, setComputing] = useState<boolean>(false);

  if (!graph.edges) {
    return (
      <Alert severity="warning">
        Анализ невозможен: данные рёбер доступны только в edgesBuffer (граф слишком большой).
      </Alert>
    );
  }

  // Построение списка смежности (мемоизация)
  const getAdjList = useCallback(() => {
    const adjList = new Map<number, Set<number>>();
    graph.edges.forEach(e => {
      if (!adjList.has(e.from)) adjList.set(e.from, new Set());
      if (!adjList.has(e.to)) adjList.set(e.to, new Set());
      adjList.get(e.from)!.add(e.to);
      if (!graph.directed) adjList.get(e.to)!.add(e.from);
    });
    return adjList;
  }, [graph]);

  // Локальный BFS для расстояния
  const calculateDistance = useCallback(
    (from: number, to: number): number => {
      const adjList = getAdjList();
      const queue = [from];
      const distances = new Map<number, number>([[from, 0]]);
      const visited = new Set<number>([from]);

      while (queue.length > 0) {
        const current = queue.shift()!;
        const dist = distances.get(current)!;
        if (current === to) return dist;

        for (const neighbor of adjList.get(current) || []) {
          if (!visited.has(neighbor)) {
            visited.add(neighbor);
            queue.push(neighbor);
            distances.set(neighbor, dist + 1);
          }
        }
      }
      return -1; // Путь не найден
    },
    [getAdjList]
  );

  // Локальный кластерный коэффициент
  const calculateClusterCoefficient = useCallback(
    (vertex: number): number => {
      const adjList = getAdjList();
      const neighbors = adjList.get(vertex) || new Set();
      const degree = neighbors.size;
      if (degree < 2) return 0;

      let triangles = 0;
      for (const n1 of neighbors) {
        for (const n2 of neighbors) {
          if (n1 < n2 && adjList.get(n1)?.has(n2)) {
            triangles++;
          }
        }
      }
      return (2 * triangles) / (degree * (degree - 1));
    },
    [getAdjList]
  );

  // Симуляция удаления узлов
  const simulateNodeRemoval = useCallback(
    (percent: number, strategy: 'random' | 'degree'): number => {
      const numToRemove = Math.floor((percent / 100) * graph.numNodes);
      let nodes = Array.from({ length: graph.numNodes }, (_, i) => i);

      if (strategy === 'degree') {
        const degrees = new Map<number, number>();
        graph.edges.forEach(e => {
          degrees.set(e.from, (degrees.get(e.from) || 0) + 1);
          degrees.set(e.to, (degrees.get(e.to) || 0) + 1);
        });
        nodes.sort((a, b) => (degrees.get(b) || 0) - (degrees.get(a) || 0));
        nodes = nodes.slice(0, numToRemove);
      } else {
        nodes = nodes.sort(() => Math.random() - 0.5).slice(0, numToRemove);
      }

      const removedNodes = new Set(nodes);
      const remainingEdges = graph.edges.filter(
        e => !removedNodes.has(e.from) && !removedNodes.has(e.to)
      );

      const adjList = new Map<number, Set<number>>();
      remainingEdges.forEach(e => {
        if (!adjList.has(e.from)) adjList.set(e.from, new Set());
        if (!adjList.has(e.to)) adjList.set(e.to, new Set());
        adjList.get(e.from)!.add(e.to);
        if (!graph.directed) adjList.get(e.to)!.add(e.from);
      });

      const visited = new Set<number>();
      let maxComponentSize = 0;

      for (let node = 0; node < graph.numNodes; node++) {
        if (!visited.has(node) && !removedNodes.has(node)) {
          const componentSize = bfsComponentSize(node, adjList, visited);
          maxComponentSize = Math.max(maxComponentSize, componentSize);
        }
      }
      return maxComponentSize / graph.numNodes;
    },
    [graph]
  );

  const bfsComponentSize = useCallback(
    (start: number, adjList: Map<number, Set<number>>, visited: Set<number>): number => {
      const queue = [start];
      visited.add(start);
      let size = 1;

      while (queue.length > 0) {
        const current = queue.shift()!;
        for (const neighbor of adjList.get(current) || []) {
          if (!visited.has(neighbor)) {
            visited.add(neighbor);
            queue.push(neighbor);
            size++;
          }
        }
      }
      return size;
    },
    []
  );

  const handleCalculateDistance = async () => {
    const from = parseInt(fromVertex);
    const to = parseInt(toVertex);
    if (isNaN(from) || isNaN(to) || from < 0 || to >= graph.numNodes) {
      setError('Некорректные ID вершин (должны быть от 0 до numNodes-1)');
      return;
    }

    setError(null);
    setComputing(true);
    try {
      const dist = calculateDistance(from, to);
      setDistance(dist);
    } finally {
      setComputing(false);
    }
  };

  const handleCalculateClusterCoeff = async () => {
    const vertex = parseInt(clusterVertex);
    if (isNaN(vertex) || vertex < 0 || vertex >= graph.numNodes) {
      setError('Некорректный ID вершины (должен быть от 0 до numNodes-1)');
      return;
    }

    setError(null);
    setComputing(true);
    try {
      const coeff = calculateClusterCoefficient(vertex);
      setClusterCoeff(coeff);
    } finally {
      setComputing(false);
    }
  };

  const handleSimulateRemoval = async () => {
    const percent = parseFloat(removePercent);
    if (isNaN(percent) || percent < 0 || percent > 100) {
      setError('Введите корректный процент (0-100)');
      return;
    }

    setError(null);
    setComputing(true);
    try {
      const fraction = simulateNodeRemoval(percent, removeStrategy);
      setComponentFraction(fraction);
    } finally {
      setComputing(false);
    }
  };

  const maxRenderNodes = 1000;
  const isLargeGraph = graph.numNodes > maxRenderNodes;
  const elements = isLargeGraph
    ? []
    : [
      ...Array.from({ length: Math.min(graph.numNodes, maxRenderNodes) }, (_, i) => ({
        data: { id: i.toString() },
      })),
      ...graph.edges.slice(0, maxRenderNodes).map((e, i) => ({
        data: {
          id: `edge-${i}`,
          source: e.from.toString(),
          target: e.to.toString(),
        },
      })),
    ];

  return (
    <ErrorBoundary>
      <Card sx={{ maxWidth: 900, mx: 'auto', mt: 4, boxShadow: 3 }}>
        <CardContent>
          <Typography variant="h5" gutterBottom>
            Анализ графа
          </Typography>
          <Box sx={{ display: 'flex', flexDirection: 'column', gap: 3 }}>
            {isLargeGraph ? (
              <Alert severity="warning">
                Граф слишком большой для визуализации. Отображается только анализ.
              </Alert>
            ) : (
              <CytoscapeComponent
                elements={elements}
                style={{ width: '100%', height: '400px' }}
                layout={{ name: 'cose', animate: true }}
                stylesheet={[
                  { selector: 'node', style: { label: 'data(id)', backgroundColor: '#1976d2' } },
                  { selector: 'edge', style: { 'curve-style': 'bezier', lineColor: '#ccc' } },
                ]}
              />
            )}

            <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>
              <Typography variant="h6">Расстояние между вершинами</Typography>
              <Box sx={{ display: 'flex', gap: 2 }}>
                <TextField
                  label="Вершина 1"
                  value={fromVertex}
                  onChange={e => setFromVertex(e.target.value)}
                  variant="outlined"
                  size="small"
                  type="number"
                />
                <TextField
                  label="Вершина 2"
                  value={toVertex}
                  onChange={e => setToVertex(e.target.value)}
                  variant="outlined"
                  size="small"
                  type="number"
                />
                <Button
                  variant="contained"
                  onClick={handleCalculateDistance}
                  disabled={computing}
                >
                  Вычислить
                </Button>
              </Box>
              {computing && <LinearProgress />}
              {distance !== null && (
                <Alert severity="info">
                  Расстояние: {distance === -1 ? 'Путь не существует' : distance}
                </Alert>
              )}
            </Box>

            <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>
              <Typography variant="h6">Локальный кластерный коэффициент</Typography>
              <Box sx={{ display: 'flex', gap: 2 }}>
                <TextField
                  label="Вершина"
                  value={clusterVertex}
                  onChange={e => setClusterVertex(e.target.value)}
                  variant="outlined"
                  size="small"
                  type="number"
                />
                <Button
                  variant="contained"
                  onClick={handleCalculateClusterCoeff}
                  disabled={computing}
                >
                  Вычислить
                </Button>
              </Box>
              {computing && <LinearProgress />}
              {clusterCoeff !== null && (
                <Alert severity="info">
                  Кластерный коэффициент: {clusterCoeff.toFixed(4)}
                </Alert>
              )}
            </Box>

            <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>
              <Typography variant="h6">Симуляция удаления узлов</Typography>
              <Box sx={{ display: 'flex', gap: 2 }}>
                <TextField
                  label="Процент удаления (%)"
                  value={removePercent}
                  onChange={e => setRemovePercent(e.target.value)}
                  variant="outlined"
                  size="small"
                  type="number"
                />
                <FormControl>
                  <FormLabel>Стратегия удаления</FormLabel>
                  <RadioGroup
                    row
                    value={removeStrategy}
                    onChange={e =>
                      setRemoveStrategy(e.target.value as 'random' | 'degree')
                    }
                  >
                    <FormControlLabel
                      value="random"
                      control={<Radio />}
                      label="Случайно"
                    />
                    <FormControlLabel
                      value="degree"
                      control={<Radio />}
                      label="По степени"
                    />
                  </RadioGroup>
                </FormControl>
                <Button
                  variant="contained"
                  onClick={handleSimulateRemoval}
                  disabled={computing}
                >
                  Симулировать
                </Button>
              </Box>
              {computing && <LinearProgress />}
              {componentFraction !== null && (
                <Alert severity="info">
                  Доля вершин в наибольшей компоненте: {componentFraction.toFixed(4)}
                </Alert>
              )}
            </Box>

            {error && <Alert severity="error">{error}</Alert>}
          </Box>
        </CardContent>
      </Card>
    </ErrorBoundary>
  );
};

export default GraphAnalyzer;