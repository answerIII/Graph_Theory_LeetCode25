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
import type { DistanceResultEstimation, MethodParams, DistanceEstimationComponentProps} from '../types/graphTypes';

// Регистрация компонентов Chart.js
ChartJS.register(CategoryScale, LinearScale, BarElement, Title, Tooltip, Legend);



const mockDistanceResults: DistanceResultEstimation[] = [
  {
    id: 'double_sweep-1',
    method: 'double_sweep',
    diameter: 10,
    percentile90: 8,
    meanDistance: 6.5,
  },
  {
    id: 'random_sample-1',
    method: 'random_sample',
    diameter: 12,
    percentile90: 9,
    meanDistance: 7.2,
  },
  {
    id: 'snowball-1',
    method: 'snowball',
    diameter: 11,
    percentile90: 7,
    meanDistance: 5.8,
  },
];

const DistanceEstimationComponent: React.FC<DistanceEstimationComponentProps> = ({ datasetname }) => {
  const [data, setData] = useState<DistanceResultEstimation[]>(mockDistanceResults);
  const [error, setError] = useState<string | null>(null);
  const [params, setParams] = useState<{
    [key: string]: MethodParams;
  }>({
    random_sample: { sampleSize: '1000' },
    snowball: { sampleSize: '1000', initialNodes: '2' },
  });

  const handleSetSampleSize = (method: string, size: number) => {
    setParams((prev) => ({
      ...prev,
      [method]: { ...prev[method], sampleSize: size.toString() },
    }));
    setError(null);
  };

  const handleCalculate = (method: 'double_sweep' | 'random_sample' | 'snowball') => {
    setError(null);
    let sampleSize = 1000; // Значение по умолчанию
    let initialNodes = 2; // Значение по умолчанию для snowball

    // Валидация для random_sample и snowball
    if (method !== 'double_sweep') {
      if (!(method in params)) {
        setError(`Некорректный метод: ${method}`);
        return;
      }
      sampleSize = parseInt(params[method].sampleSize) || 1000;
      if (sampleSize < 500 || sampleSize > 1000) {
        setError('Размер выборки должен быть от 500 до 1000');
        return;
      }
      if (method === 'snowball') {
        initialNodes = parseInt(params[method].initialNodes || '2');
        if (![2, 3].includes(initialNodes)) {
          setError('Начальное количество вершин должно быть 2 или 3');
          return;
        }
      }
    }

    // Имитация вычисления
    const newResult: DistanceResultEstimation = {
      id: `${method}-${Date.now()}`,
      method,
      diameter: Math.floor(Math.random() * 5) + 8, // 8–12
      percentile90: Math.floor(Math.random() * 4) + 6, // 6–9
      meanDistance: Math.random() * 3 + 5, // 5–8
    };

    setData((prev) =>
      prev.map((r) => (r.method === newResult.method ? newResult : r))
    );
    setError(`Вычислено для ${method} (статические данные)`);

    // Закомментированный код для бэкенда
    /*
    try {
      const response = await fetch(`/api/graphs/${datasetname}/distances`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          method,
          sampleSize: method !== 'double_sweep' ? sampleSize : undefined,
          initialNodes: method === 'snowball' ? initialNodes : undefined,
        }),
      });
      if (!response.ok) throw new Error('Ошибка сервера');
      const data: DistanceResultEstimation = await response.json();
      if (!data.diameter || !data.percentile90 || !data.meanDistance) {
        throw new Error('Некорректный формат ответа от API');
      }
      setData((prev) =>
        prev.map((r) => (r.method === data.method ? data : r))
      );
      setError(`Вычислено для ${data.method}`);
    } catch (err) {
      setError(`Ошибка вычисления (${method}): ${err.message}`);
    }
    */
  };

  const chartData = {
    labels: ['Double Sweep', 'Random Sample', 'Snowball'],
    datasets: [
      {
        label: 'Диаметр',
        data: data.map((r) => r.diameter),
        backgroundColor: '#1976d2',
        borderColor: '#1565c0',
        borderWidth: 1,
      },
      {
        label: '90-й процентиль',
        data: data.map((r) => r.percentile90),
        backgroundColor: '#d32f2f',
        borderColor: '#b71c1c',
        borderWidth: 1,
      },
      {
        label: 'Среднее расстояние',
        data: data.map((r) => r.meanDistance),
        backgroundColor: '#388e3c',
        borderColor: '#2e7d32',
        borderWidth: 1,
      },
    ],
  };

  const chartOptions = {
    scales: {
      y: { beginAtZero: true, title: { display: true, text: 'Значение' } },
      x: { title: { display: true, text: 'Метод' } },
    },
    plugins: {
      legend: { display: true, position: 'bottom' as const },
    },
  };

  if (!datasetname) {
    return <Alert severity="error">Dataset name is not provided</Alert>;
  }

  return (
  <Box sx={{ p: 3, display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
    <Typography variant="h6" gutterBottom sx={{ textAlign: 'center' }}>
      Оценка расстояний
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
      {['double_sweep', 'random_sample', 'snowball'].map((method) => (
        <Card
          key={method}
          sx={{
            boxShadow: 3,
            minWidth: 300,
            maxWidth: 350,
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
              {method === 'double_sweep'
                ? 'Double Sweep'
                : method === 'random_sample'
                ? 'Random Sample'
                : 'Snowball'}
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
              {method !== 'double_sweep' && (
                <Box sx={{ display: 'flex', flexDirection: 'column', gap: 1, alignItems: 'center' }}>
                  <Box sx={{ display: 'flex', gap: 1, alignItems: 'center' }}>
                    <TextField
                      label="Размер выборки"
                      value={params[method]?.sampleSize ?? '1000'}
                      onChange={(e) =>
                        setParams((prev) => ({
                          ...prev,
                          [method]: { ...prev[method], sampleSize: e.target.value },
                        }))
                      }
                      type="number"
                      size="small"
                      sx={{ width: 120 }}
                      helperText="500–1000"
                    />
                    <ButtonGroup size="small" variant="outlined">
                      <Button onClick={() => handleSetSampleSize(method, 500)}>500</Button>
                      <Button onClick={() => handleSetSampleSize(method, 1000)}>1000</Button>
                    </ButtonGroup>
                  </Box>
                  {method === 'snowball' && (
                    <Box sx={{ display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
                      <Typography variant="caption" sx={{ mb: 0.5 }}>
                        Начальные вершины
                      </Typography>
                      <ButtonGroup size="small" variant="outlined">
                        <Button
                          onClick={() =>
                            setParams((prev) => ({
                              ...prev,
                              [method]: { ...prev[method], initialNodes: '2' },
                            }))
                          }
                          sx={{
                            bgcolor: params[method]?.initialNodes === '2' ? 'action.selected' : 'inherit',
                          }}
                        >
                          2
                        </Button>
                        <Button
                          onClick={() =>
                            setParams((prev) => ({
                              ...prev,
                              [method]: { ...prev[method], initialNodes: '3' },
                            }))
                          }
                          sx={{
                            bgcolor: params[method]?.initialNodes === '3' ? 'action.selected' : 'inherit',
                          }}
                        >
                          3
                        </Button>
                      </ButtonGroup>
                    </Box>
                  )}
                </Box>
              )}
              <Button
                variant="contained"
                size="small"
                onClick={() =>
                  handleCalculate(method as 'double_sweep' | 'random_sample' | 'snowball')
                }
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
      <Table sx={{ mt: 3, maxWidth: 800, width: '100%' }}>
        <TableHead>
          <TableRow>
            <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Метод</TableCell>
            <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Диаметр</TableCell>
            <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>90-й процентиль</TableCell>
            <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>
              Среднее расстояние
            </TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {data.map((result) => (
            <TableRow key={result.id}>
              <TableCell sx={{ textAlign: 'center' }}>
                {result.method === 'double_sweep'
                  ? 'Double Sweep'
                  : result.method === 'random_sample'
                  ? 'Random Sample'
                  : 'Snowball'}
              </TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{result.diameter}</TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{result.percentile90.toFixed(2)}</TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{result.meanDistance.toFixed(2)}</TableCell>
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
        Сравнение результатов
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

export default DistanceEstimationComponent;