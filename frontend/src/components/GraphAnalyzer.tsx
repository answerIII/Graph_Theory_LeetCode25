import React, { useState } from 'react';
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
  FormLabel
} from '@mui/material';
import CytoscapeComponent from 'react-cytoscapejs';
import type { Graph } from '../types/graph';

interface GraphAnalyzerProps {
  graph: Graph;
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

  // Локальный BFS для расстояния
  const calculateDistance = (from: number, to: number): number => {
    
    const adjList = new Map<number, number[]>();
    
    graph.edges.forEach(e => {
      if (!adjList.has(e.from)) adjList.set(e.from, []);
      if (!adjList.has(e.to)) adjList.set(e.to, []);
      
      adjList.get(e.from)!.push(e.to);
      
      if (!graph.directed) adjList.get(e.to)!.push(e.from);
    });

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
  };

  // Локальный кластерный коэффициент
  const calculateClusterCoefficient = (vertex: number): number => {
    
    const adjList = new Map<number, number[]>();
    
    graph.edges.forEach(e => {
      
      if (!adjList.has(e.from)) adjList.set(e.from, []);
      if (!adjList.has(e.to)) adjList.set(e.to, []);
      
      adjList.get(e.from)!.push(e.to);
      
      if (!graph.directed) adjList.get(e.to)!.push(e.from);
    });

    const neighbors = adjList.get(vertex) || [];
    const degree = neighbors.length;
    
    if (degree < 2) return 0;

    let triangles = 0;
    
    for (const n1 of neighbors) {
      for (const n2 of neighbors) {
        if (n1 < n2 && adjList.get(n1)?.includes(n2)) {
          triangles++;
        }
      }
    }

    return (2 * triangles) / (degree * (degree - 1));
  };

  // Симуляция удаления узлов
  const simulateNodeRemoval = (percent: number, strategy: 'random' | 'degree'): number => {
    
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
    } 
    else {
      nodes = nodes.sort(() => Math.random() - 0.5).slice(0, numToRemove);
    }

    const remainingEdges = graph.edges.filter(
      e => !nodes.includes(e.from) && !nodes.includes(e.to)
    );
    
    const adjList = new Map<number, number[]>();
    
    remainingEdges.forEach(e => {
      
      if (!adjList.has(e.from)) adjList.set(e.from, []);
      if (!adjList.has(e.to)) adjList.set(e.to, []);
      
      adjList.get(e.from)!.push(e.to);
      
      if (!graph.directed) adjList.get(e.to)!.push(e.from);
    });

    const visited = new Set<number>();
    
    let maxComponentSize = 0;
    
    for (let node = 0; node < graph.numNodes; node++) {
      
      if (!visited.has(node) && !nodes.includes(node)) {
        
        const componentSize = bfsComponentSize(node, adjList, visited);
        maxComponentSize = Math.max(maxComponentSize, componentSize);
      }
    }
    return maxComponentSize / graph.numNodes;
  };

  const bfsComponentSize = (
    start: number,
    adjList: Map<number, number[]>,
    visited: Set<number>
  ): number => {
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
  };

  const handleCalculateDistance = () => {
    
    const from = parseInt(fromVertex);
    const to = parseInt(toVertex);
    
    if (isNaN(from) || isNaN(to) || from < 0 || to < 0 || from >= graph.numNodes || to >= graph.numNodes) {
      setError('Некорректные ID вершин');
      return;
    }
    
    setError(null);
    const dist = calculateDistance(from, to);
    setDistance(dist);
  };

  const handleCalculateClusterCoeff = () => {
    
    const vertex = parseInt(clusterVertex);
   
    if (isNaN(vertex) || vertex < 0 || vertex >= graph.numNodes) {
      setError('Некорректный ID вершины');
      return;
    }
    
    setError(null);
    const coeff = calculateClusterCoefficient(vertex);
    setClusterCoeff(coeff);
  };

  const handleSimulateRemoval = () => {
    
    const percent = parseFloat(removePercent);
    
    if (isNaN(percent) || percent < 0 || percent > 100) {
      setError('Введите корректный процент (0-100)');
      return;
    }
    
    setError(null);
    const fraction = simulateNodeRemoval(percent, removeStrategy);
    setComponentFraction(fraction);
  };

  const maxRenderNodes = 1000;
  const isLargeGraph = graph.numNodes > maxRenderNodes;
  const elements = isLargeGraph
    ? []
    : [
        ...Array.from({ length: Math.min(graph.numNodes, maxRenderNodes) }, (_, i) => ({
          data: { id: i.toString() },
        })),
        ...graph.edges.slice(0, maxRenderNodes).map(e => ({
          data: { source: e.from.toString(), target: e.to.toString() },
        })),
      ];

  return (
    <Card sx={{ maxWidth: 900, mx: 'auto', mt: 4, boxShadow: 3 }}>
      
      <CardContent>
        
        <Typography variant="h5" gutterBottom>
          Анализ графа
        </Typography>
        
        <Box sx={{ display: 'flex', flexDirection: 'column', gap: 3 }}>
          {isLargeGraph ? (
            <Alert severity="warning">Граф слишком большой для визуализации. Отображается только анализ.</Alert>
          ) : (
            <CytoscapeComponent
              elements={elements}
              style={{ width: '100%', height: '400px' }}
              layout={{ name: 'circle' }}
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
              
              <Button variant="contained" onClick={handleCalculateDistance}>
                Вычислить
              </Button>
            </Box>
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
              
              <Button variant="contained" onClick={handleCalculateClusterCoeff}>
                Вычислить
              </Button>
            </Box>
            {clusterCoeff !== null && (
              <Alert severity="info">Кластерный коэффициент: {clusterCoeff.toFixed(4)}</Alert>
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
                  onChange={e => setRemoveStrategy(e.target.value as 'random' | 'degree')}
                >
                  <FormControlLabel value="random" control={<Radio />} label="Случайно" />
                  <FormControlLabel value="degree" control={<Radio />} label="По степени" />
                </RadioGroup>
              </FormControl>

              <Button variant="contained" onClick={handleSimulateRemoval}>
                Симулировать
              </Button>

            </Box>
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
  );
};

export default GraphAnalyzer;
