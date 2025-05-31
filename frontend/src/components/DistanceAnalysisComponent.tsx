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
import { saveAs } from 'file-saver';
import { graphApi } from '../api/graphApi';
import type { DistanceResultAnalysis, AlgorithmParams } from '../types/graphTypes';

ChartJS.register(CategoryScale, LinearScale, BarElement, Title, Tooltip, Legend);

const mockDistanceResults: DistanceResultAnalysis[] = [
  {
    algorithm: 'bfs',
    distance: 5,
    execution_time: 1185,
    landmarks: [],
    start_node: 100,
    end_node: 200,
  },
  {
    algorithm: 'landmarks-basic',
    distance: 6,
    execution_time: 900,
    landmarks: [123, 456, 789],
    start_node: 100,
    end_node: 200,
  },
  {
    algorithm: 'landmarks-bfs',
    distance: 6,
    execution_time: 950,
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
    bfs: { start_node: '', end_node: '' },
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

  const handleCalculate = async (algorithm: 'bfs' | 'landmarks-basic' | 'landmarks-bfs') => {
    setError(null);
    if (!(algorithm in params)) {
      setError(`Некорректный алгоритм: ${algorithm}`);
      return;
    }
    const algoParams = params[algorithm];
    const startNode = parseInt(algoParams.start_node) || -1;
    const endNode = parseInt(algoParams.end_node) || -1;
    let landmarksCount = algorithm !== 'bfs' ? parseInt(algoParams.landmarks_count) || 10 : undefined;

    if (startNode !== -1 && startNode < 0) {
      setError('Начальная вершина должна быть неотрицательной или -1');
      return;
    }
    if (endNode !== -1 && endNode < 0) {
      setError('Конечная вершина должна быть неотрицательной или -1');
      return;
    }
    if (algorithm !== 'bfs' && (landmarksCount <= 0 || !Number.isInteger(landmarksCount))) {
      setError('Количество ориентиров должно быть целым числом больше 0');
      return;
    }

    try {
      const payload: any = {
        start_node: startNode,
        end_node: endNode,
        algorithm,
      };
      if (algorithm !== 'bfs') {
        payload.landmarks = {
          count: landmarksCount,
          selection: algoParams.landmarks_selection,
        };
      }
      const response = await graphApi.calculateDistance(datasetname, payload);
      if (!response.algorithm || !('distance' in response) || !Array.isArray(response.landmarks)) {
        throw new Error('Некорректный формат ответа от API');
      }
      setData((prev) =>
        prev.map((r) => (r.algorithm === response.algorithm ? response : r))
      );
      setError(`Вычислено для ${response.algorithm}`);
    } catch (err) {
      setError(`Ошибка вычисления (${algorithm}): ${(err as Error).message}`);
    }
  };

  const handleDownloadCsv = () => {
    const csvContent = [
      ['Алгоритм', 'Расстояние', 'Время (мс)', 'Ориентиры', 'Начальная вершина', 'Конечная вершина'],
      ...data.map((result) => [
        result.algorithm,
        result.distance ?? 'N/A',
        result.execution_time,
        result.landmarks.join(';') || '-',
        result.start_node,
        result.end_node,
      ]),
    ]
      .map(row => row.join(','))
      .join('\n');
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
    saveAs(blob, `${datasetname}_distance_analysis.csv`);
  };

  const distanceChartData = {
    labels: ['bfs', 'landmarks-basic', 'landmarks-bfs'],
    datasets: [
      {
        label: 'Расстояние',
        data: data.map((r) => r.distance ?? 0),
        backgroundColor: '#1976d2',
        borderColor: '#1565c0',
        borderWidth: 1,
      },
    ],
  };

  const timeChartData = {
    labels: ['bfs', 'landmarks-basic', 'landmarks-bfs'],
    datasets: [
      {
        label: 'Время выполнения (мс)',
        data: data.map((r) => r.execution_time),
        backgroundColor: '#f57c00',
        borderColor: '#ef6c00',
        borderWidth: 1,
      },
    ],
  };

  const distanceChartOptions = {
    scales: {
      y: { beginAtZero: true, title: { display: true, text: 'Расстояние' } },
      x: { title: { display: true, text: 'Алгоритм' } },
    },
    plugins: { legend: { display: false } },
  };

  const timeChartOptions = {
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
                {algorithm}
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
                    helperText="-1 для случайной"
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
                    helperText="-1 для случайной"
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
                      helperText="Целое число > 0"
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
                  sx={{ mt: 'auto', width: '100%' }}
                >
                  Вычислить
                </Button>
              </Box>
            </CardContent>
          </Card>
        ))}
      </Box>
      {data.length > 0 ? (
        <>
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
                <TableRow key={result.algorithm}>
                  <TableCell sx={{ textAlign: 'center' }}>{result.algorithm}</TableCell>
                  <TableCell sx={{ textAlign: 'center' }}>{result.distance ?? 'N/A'}</TableCell>
                  <TableCell sx={{ textAlign: 'center' }}>{result.execution_time}</TableCell>
                  <TableCell sx={{ textAlign: 'center' }}>{result.landmarks.join(', ') || '-'}</TableCell>
                  <TableCell sx={{ textAlign: 'center' }}>{result.start_node}</TableCell>
                  <TableCell sx={{ textAlign: 'center' }}>{result.end_node}</TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
          <Button variant="outlined" onClick={handleDownloadCsv} sx={{ mt: 2 }}>
            Скачать CSV
          </Button>
        </>
      ) : (
        <Alert severity="warning" sx={{ mt: 3, maxWidth: 800, width: '100%' }}>
          Нет данных для отображения
        </Alert>
      )}
      <Box sx={{ mt: 3, width: '100%', display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
        <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
          Сравнение расстояний
        </Typography>
        {data.length > 0 ? (
          <Box sx={{ minWidth: 600, height: 300, margin: '0 auto', mb: 4 }}>
            <Bar data={distanceChartData} options={distanceChartOptions} />
          </Box>
        ) : (
          <Alert severity="warning" sx={{ minWidth: 800, width: '100%', mb: 4 }}>
            Нет данных для гистограммы расстояний
          </Alert>
        )}
        <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
          Сравнение времени выполнения
        </Typography>
        {data.length > 0 ? (
          <Box sx={{ minWidth: 600, height: 300, margin: '0 auto' }}>
            <Bar data={timeChartData} options={timeChartOptions} />
          </Box>
        ) : (
          <Alert severity="warning" sx={{ minWidth: 800, width: '100%' }}>
            Нет данных для гистограммы времени
          </Alert>
        )}
      </Box>
    </Box>
  );
};

export default DistanceAnalysisComponent;