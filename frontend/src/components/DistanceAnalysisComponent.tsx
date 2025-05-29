import React, { useState } from 'react';
import {
  Box,
  Button,
  ButtonGroup,
  TextField,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  Typography,
  Alert,
  Card,
  CardContent,
} from '@mui/material';
import { Bar } from 'react-chartjs-2';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  BarElement,
  Title,
  Tooltip,
  Legend,
} from 'chart.js';

import type { DistanceResultAnalysis, AlgorithmParams } from '../types/graphTypes';

// Регистрация компонентов Chart.js
ChartJS.register(CategoryScale, LinearScale, BarElement, Title, Tooltip, Legend);

const mockDistanceResults: DistanceResultAnalysis[] = [
  {
    id: 'bfs-1',
    algorithm: 'BFS',
    distance: 5,
    execution_time_ms: 1185,
    landmarks: [],
    start_node: 100,
    end_node: 200,
  },
  {
    id: 'landmarks-basic-1',
    algorithm: 'Landmarks-Basic',
    distance: 6,
    execution_time_ms: 0,
    landmarks: [123, 456, 789],
    start_node: 100,
    end_node: 200,
  },
  {
    id: 'landmarks-bfs-1',
    algorithm: 'Landmarks-BFS',
    distance: 6,
    execution_time_ms: 9,
    landmarks: [123, 456, 789],
    start_node: 100,
    end_node: 200,
  },
];

const DistanceAnalysisComponent: React.FC<{ datasetname: string | undefined }> = ({ datasetname }) => {
  const [data, setData] = useState<DistanceResultAnalysis[]>(mockDistanceResults);
  const [error, setError] = useState<string | null>(null);
  const [params, setParams] = useState<{
    [key: string]: AlgorithmParams;
  }>({
    bfs: { start_node: '', end_node: '', landmarks_count: '10', landmarks_selection: 'random' },
    'landmarks-basic': { start_node: '', end_node: '', landmarks_count: '10', landmarks_selection: 'random' },
    'landmarks-bfs': { start_node: '', end_node: '', landmarks_count: '10', landmarks_selection: 'random' },
  });

  const handleRandomNodes = (algorithm: string) => {
    if (!(algorithm in params)) {
      setError(`Некорректный алгоритм: ${algorithm}`);
      return;
    }
    setParams((prev) => ({
      ...prev,
      [algorithm]: { ...prev[algorithm], start_node: '-1', end_node: '-1' },
    }));
    setError(null);
  };

  const handleCalculate = (algorithm: string) => {
    setError(null);
    if (!(algorithm in params)) {
      setError(`Некорректный алгоритм: ${algorithm}`);
      return;
    }
    const algoParams = params[algorithm];
    const startNode = parseInt(algoParams.start_node) || -1;
    const endNode = parseInt(algoParams.end_node) || -1;
    const landmarksCount = parseInt(algoParams.landmarks_count) || 10;

    // Валидация
    if (startNode !== -1 && startNode < 0) {
      setError('Начальная вершина должна быть неотрицательной');
      return;
    }
    if (endNode !== -1 && endNode < 0) {
      setError('Конечная вершина должна быть неотрицательной');
      return;
    }
    if (algorithm !== 'bfs' && landmarksCount <= 0) {
      setError('Количество ориентиров должно быть больше 0');
      return;
    }

    // Имитация вычисления
    const newResult: DistanceResultAnalysis = {
      id: `${algorithm}-${Date.now()}`,
      algorithm: algorithm === 'bfs' ? 'BFS' : algorithm === 'landmarks-basic' ? 'Landmarks-Basic' : 'Landmarks-BFS',
      distance: Math.floor(Math.random() * 10),
      execution_time_ms: Math.floor(Math.random() * 2000),
      landmarks: algorithm === 'bfs' ? [] : Array.from({ length: landmarksCount }, () => Math.floor(Math.random() * 1000)),
      start_node: startNode,
      end_node: endNode,
    };

    setData((prev) =>
      prev.map((r) => (r.algorithm === newResult.algorithm ? newResult : r))
    );
    setError(`Вычислено для ${newResult.algorithm} (статические данные)`);

    // Закомментированный код для бэкенда
    /*
    try {
      const response = await graphApi.calculateDistance(datasetname, {
        start_node: startNode,
        end_node: endNode,
        algorithm,
        landmarks: {
          count: landmarksCount,
          selection: algoParams.landmarks_selection,
        },
      });
      if (!response || !response.algorithm || !('distance' in response)) {
        throw new Error('Некорректный формат ответа от API');
      }
      setData((prev) =>
        prev.map((r) => (r.algorithm === response.algorithm ? response : r))
      );
      setError(`Вычислено для ${response.algorithm}`);
    } catch (err) {
      setError(`Ошибка вычисления (${algorithm}): ` + (err as Error).message);
    }
    */
  };

  const chartData = {
    labels: ['BFS', 'Landmarks-Basic', 'Landmarks-BFS'],
    datasets: [
      {
        label: 'Время выполнения (мс)',
        data: data.map((r) => r.execution_time_ms),
        backgroundColor: ['#1976d2', '#d32f2f', '#388e3c'],
        borderColor: ['#1565c0', '#b71c1c', '#2e7d32'],
        borderWidth: 1,
      },
    ],
  };

  const chartOptions = {
    scales: {
      y: { beginAtZero: true, title: { display: true, text: 'Время (мс)' } },
      x: { title: { display: true, text: 'Алгоритм' } },
    },
    plugins: { legend: { display: false } },
  };

  if (!datasetname) {
    return <Alert severity="error">Dataset name is not provided</Alert>;
  }

  return (
    <Box sx={{ p: 3, display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
      <Typography variant="h6" gutterBottom sx={{ textAlign: 'center' }}>
        Вычисление расстояний
      </Typography>
      {error && (
        <Alert
          severity={error.includes('Вычислено') ? 'info' : 'error'}
          sx={{ width: '100%', maxWidth: 800, mb: 2 }}
        >
          {error}
        </Alert>
      )}
      <Box
        sx={{
          display: 'flex',
          flexDirection: 'row',
          gap: 3,
          mt: 2,
          overflowX: 'auto',
          pb: 2,
          justifyContent: 'center',
          width: '100%',
        }}
      >
        {['bfs', 'landmarks-basic', 'landmarks-bfs'].map((algorithm) => (
          <Card
            key={algorithm}
            sx={{
              boxShadow: 3,
              minWidth: 300,
              maxWidth: 330,
              flexShrink: 0,
              display: 'flex',
              flexDirection: 'column',
              marginTop: '5px',
            }}
          >
            <CardContent
              sx={{
                flexGrow: 1,
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'center',
                gap: 2,
              }}
            >
              <Typography variant="subtitle1" sx={{ textAlign: 'center' }}>
                {algorithm === 'bfs' ? 'BFS' : algorithm === 'landmarks-basic' ? 'Landmarks-Basic' : 'Landmarks-BFS'}
              </Typography>
              <Box
                sx={{
                  display: 'flex',
                  flexDirection: 'column',
                  gap: 2,
                  alignItems: 'center',
                  width: '100%',
                  flexGrow: 1,
                }}
              >
                <Box sx={{ display: 'flex', gap: 1, alignItems: 'center', flexWrap: 'wrap', justifyContent: 'center' }}>
                  <TextField
                    label="Начальная вершина"
                    value={params[algorithm]?.start_node ?? ''}
                    onChange={(e) =>
                      setParams((prev) => ({
                        ...prev,
                        [algorithm]: { ...prev[algorithm], start_node: e.target.value },
                      }))
                    }
                    type="number"
                    size="small"
                    sx={{ width: 125 }}
                  />
                  <TextField
                    label="Конечная вершина"
                    value={params[algorithm]?.end_node ?? ''}
                    onChange={(e) =>
                      setParams((prev) => ({
                        ...prev,
                        [algorithm]: { ...prev[algorithm], end_node: e.target.value },
                      }))
                    }
                    type="number"
                    size="small"
                    sx={{ width: 124 }}
                  />
                  <Button
                    variant="outlined"
                    size="small"
                    onClick={() => handleRandomNodes(algorithm)}
                  >
                    Случайные вершины
                  </Button>
                </Box>
                {algorithm !== 'bfs' && (
                  <Box sx={{ display: 'flex', flexDirection: 'column', gap: 1, alignItems: 'center' }}>
                    <TextField
                      label="Количество ориентиров"
                      value={params[algorithm]?.landmarks_count ?? '10'}
                      onChange={(e) =>
                        setParams((prev) => ({
                          ...prev,
                          [algorithm]: { ...prev[algorithm], landmarks_count: e.target.value },
                        }))
                      }
                      type="number"
                      size="small"
                      sx={{ width: '100%' }}
                    />
                    <Box sx={{ display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
                      <Typography variant="caption" sx={{ mb: 0.5 }}>
                        Выбор ориентиров
                      </Typography>
                      <ButtonGroup size="small" variant="outlined">
                        <Button
                          onClick={() =>
                            setParams((prev) => ({
                              ...prev,
                              [algorithm]: { ...prev[algorithm], landmarks_selection: 'random' },
                            }))
                          }
                          sx={{
                            bgcolor: params[algorithm]?.landmarks_selection === 'random' ? 'action.selected' : 'inherit',
                            fontSize: '10px'
                          }}
                        >
                          Случайные
                        </Button>
                        <Button
                          onClick={() =>
                            setParams((prev) => ({
                              ...prev,
                              [algorithm]: { ...prev[algorithm], landmarks_selection: 'highest_degree' },
                            }))
                          }
                          sx={{
                            bgcolor: params[algorithm]?.landmarks_selection === 'highest_degree' ? 'action.selected' : 'inherit',
                            fontSize: '10px'
                          }}
                        >
                          Наиб. степени
                        </Button>
                        <Button
                          onClick={() =>
                            setParams((prev) => ({
                              ...prev,
                              [algorithm]: { ...prev[algorithm], landmarks_selection: 'max_coverage' },
                            }))
                          }
                          sx={{
                            bgcolor: params[algorithm]?.landmarks_selection === 'max_coverage' ? 'action.selected' : 'inherit',
                            fontSize: '10px'
                          }}
                        >
                          Макс. покрытие
                        </Button>
                      </ButtonGroup>
                    </Box>
                  </Box>
                )}
                <Button
                  variant="contained"
                  size="small"
                  onClick={() => handleCalculate(algorithm)}
                  sx={{ mt: 'auto', width: '100%'}}
                >
                  Вычислить
                </Button>
              </Box>
            </CardContent>
          </Card>
        ))}
      </Box>
      {data.length > 0 ? (
        <Table sx={{ mt: 3, maxWidth: 800, width: '100%' }}>
          <TableHead>
            <TableRow>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Алгоритм</TableCell>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Расстояние</TableCell>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Время (мс)</TableCell>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Ориентиры</TableCell>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Начальная вершина</TableCell>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Конечная вершина</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {data.map((result) => (
              <TableRow key={result.id}>
                <TableCell sx={{ textAlign: 'center' }}>{result.algorithm}</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{result.distance ?? 'N/A'}</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{result.execution_time_ms}</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{result.landmarks.join(', ') || '-'}</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{result.start_node}</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{result.end_node}</TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      ) : (
        <Alert severity="warning" sx={{ mt: 3, maxWidth: 800, width: '100%' }}>
          Нет данных для отображения
        </Alert>
      )}
      {/* <Button variant="outlined" disabled sx={{ mt: 2 }}>
        Скачать CSV
      </Button> */}
      <Box sx={{ mt: 3, width: '100%', display: 'flex', justifyContent: 'center' }}>
        <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
          Сравнение времени выполнения
        </Typography>
      </Box>
      {data.length > 0 ? (
        <Box sx={{ minWidth: 600, height: 300, margin: '0 auto' }}>
          <Bar data={chartData} options={chartOptions} />
        </Box>
      ) : (
        <Alert severity="warning" sx={{ minWidth: 800, width: '100%' }}>
          Нет данных для гистограммы
        </Alert>
      )}
    </Box>
  );
};

export default DistanceAnalysisComponent;
