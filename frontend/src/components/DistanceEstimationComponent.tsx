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
import type { DistanceResultEstimation, MethodParams, DistanceEstimationComponentProps } from '../types/graphTypes';

// Регистрация компонентов Chart.js
ChartJS.register(CategoryScale, LinearScale, BarElement, Title, Tooltip, Legend);

const mockDistanceResults: DistanceResultEstimation[] = [
  {
    method: 'random_sample',
    diameter: 12,
    percentile90: 9,
    meanDistance: 7.2,
    execution_time_ms: 1200,
  },
  {
    method: 'snowball',
    diameter: 11,
    percentile90: 7,
    meanDistance: 5.8,
    execution_time_ms: 1500,
  },
  {
    method: 'random_sample_snowball',
    diameter: 11,
    percentile90: 8,
    meanDistance: 6.5,
    execution_time_ms: 1800,
  },
];

const DistanceEstimationComponent: React.FC<DistanceEstimationComponentProps> = ({ datasetname }) => {
  const [data, setData] = useState<DistanceResultEstimation[]>(mockDistanceResults);
  const [error, setError] = useState<string | null>(null);
  const [params, setParams] = useState<{
    [key: string]: MethodParams;
  }>({
    random_sample: { sampleSize: '1000' },
    snowball: { sampleSize: '1000' },
    random_sample_snowball: { sampleSize: '1000' },
  });

  const handleSetSampleSize = (method: string, size: number) => {
    setParams((prev) => ({
      ...prev,
      [method]: { sampleSize: size.toString() },
    }));
    setError(null);
  };

  const handleCalculate = async (method: 'random_sample' | 'snowball' | 'random_sample_snowball') => {
    setError(null);
    if (!(method in params)) {
      setError(`Некорректный метод: ${method}`);
      return;
    }
    const sampleSize = parseInt(params[method].sampleSize) || 1000;
    if (sampleSize < 500 || sampleSize > 1000) {
      setError('Размер выборки должен быть от 500 до 1000');
      return;
    }

    // Имитация вычисления
    // const newResult: DistanceResultEstimation = {
    //   method,
    //   diameter: Math.floor(Math.random() * 5) + 8, // 8–12
    //   percentile90: Math.floor(Math.random() * 4) + 6, // 6–9
    //   meanDistance: Math.random() * 3 + 5, // 5–8
    //   execution_time_ms: Math.floor(Math.random() * 1000) + 1000, // 1000–2000
    // };

    // setData((prev) =>
    //   prev.map((r) => (r.method === newResult.method ? newResult : r))
    // );
    // setError(`Вычислено для ${method} (статические данные)`);

    // Реальный запрос

    try {
      const response = await graphApi.getDistances(datasetname, method, sampleSize);
      if (!response.diameter || !response.percentile90 || !response.meanDistance || !response.execution_time_ms) {
        throw new Error('Некорректный формат ответа от API');
      }
      setData((prev) =>
        prev.map((r) => (r.method === response.method ? response : r))
      );
      setError(`Вычислено для ${response.method}`);
    } catch (err) {
      setError(`Ошибка вычисления (${method}): ${(err as Error).message}`);
    }

  };

  const handleDownloadCsv = () => {
    const csvContent = [
      ['Метод', 'Диаметр', '90-й процентиль', 'Среднее расстояние', 'Время (мс)'],
      ...data.map((result) => [
        result.method === 'random_sample' ? 'Random Sample' : result.method === 'snowball' ? 'Snowball' : 'Random Sample & Snowball',
        result.diameter,
        result.percentile90.toFixed(2),
        result.meanDistance.toFixed(2),
        result.execution_time_ms,
      ]),
    ]
      .map(row => row.join(','))
      .join('\n');
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
    saveAs(blob, `${datasetname}_distance_estimation.csv`);
  };


  const metricsChartData = {
    labels: ['Random Sample', 'Snowball', 'Random Sample & Snowball'],
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

  const timeChartData = {
    labels: ['Random Sample', 'Snowball', 'Random Sample & Snowball'],
    datasets: [
      {
        label: 'Время выполнения (мс)',
        data: data.map((r) => r.execution_time_ms),
        backgroundColor: '#f57c00',
        borderColor: '#ef6c00',
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

  const timeChartOptions = {
    scales: {
      y: { beginAtZero: true, title: { display: true, text: 'Время (мс)' } },
      x: { title: { display: true, text: 'Метод' } },
    },
    plugins: {
      legend: { display: false },
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
        {['random_sample', 'snowball', 'random_sample_snowball'].map((method) => (
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
                {method === 'random_sample'
                  ? 'Random Sample'
                  : method === 'snowball'
                    ? 'Snowball'
                    : 'Random Sample & Snowball'}
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
                <Box sx={{ display: 'flex', gap: 1, alignItems: 'center' }}>
                  <TextField
                    label="Размер выборки"
                    value={params[method]?.sampleSize ?? '1000'}
                    onChange={(e) =>
                      setParams((prev) => ({
                        ...prev,
                        [method]: { sampleSize: e.target.value },
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
                <Button
                  variant="contained"
                  size="small"
                  onClick={() =>
                    handleCalculate(method as 'random_sample' | 'snowball' | 'random_sample_snowball')
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
        <>
        <Table sx={{ mt: 3, maxWidth: 800, width: '100%' }}>
          <TableHead>
            <TableRow>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Метод</TableCell>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Диаметр</TableCell>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>90-й процентиль</TableCell>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Среднее расстояние</TableCell>
              <TableCell sx={{ textAlign: 'center', fontWeight: 'bold' }}>Время (мс)</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {data.map((result) => (
              <TableRow key={result.method}>
                <TableCell sx={{ textAlign: 'center' }}>
                  {result.method === 'random_sample'
                    ? 'Random Sample'
                    : result.method === 'snowball'
                      ? 'Snowball'
                      : 'Random Sample & Snowball'}
                </TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{result.diameter}</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{result.percentile90.toFixed(2)}</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{result.meanDistance.toFixed(2)}</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{result.execution_time_ms}</TableCell>
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
          Сравнение метрик
        </Typography>
        {data.length > 0 ? (
          <Box sx={{ minWidth: 600, height: 300, margin: '0 auto', mb: 4 }}>
            <Bar data={metricsChartData} options={chartOptions} />
          </Box>
        ) : (
          <Alert severity="warning" sx={{ minWidth: 800, width: '100%', mb: 4 }}>
            Нет данных для гистограммы метрик
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

export default DistanceEstimationComponent;