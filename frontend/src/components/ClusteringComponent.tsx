import React, { useState, useEffect } from 'react';
import {
  Box,
  Typography,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  Alert,
  CircularProgress,
  Tooltip,
  Button,
} from '@mui/material';
import { Bar } from 'react-chartjs-2';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  BarElement,
  ArcElement,
  Title,
  Tooltip as ChartTooltip,
  Legend,
} from 'chart.js';
import { graphApi } from '../api/graphApi';
import type { ClusteringData } from '../types/graphTypes';
import { saveAs } from 'file-saver';

ChartJS.register(CategoryScale, LinearScale, BarElement, ArcElement, Title, ChartTooltip, Legend);

interface ClusteringComponentProps {
  datasetname: string | undefined;
}

const ClusteringComponent: React.FC<ClusteringComponentProps> = ({ datasetname }) => {
  const [data, setData] = useState<ClusteringData | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    const fetchData = async () => {
      if (!datasetname) {
        setError('Dataset name is not provided');
        setLoading(false);
        return;
      }
      try {
        setLoading(true);
        setError(null);
        const response = await graphApi.getClustering(datasetname);
        if (
          typeof response.trianglesCount?.value !== 'number' ||
          typeof response.trianglesCount?.execution_time_ms !== 'number' ||
          typeof response.globalClusteringCoef?.value !== 'number' ||
          typeof response.globalClusteringCoef?.execution_time_ms !== 'number' ||
          typeof response.avgClusteringCoef?.value !== 'number' ||
          typeof response.avgClusteringCoef?.execution_time_ms !== 'number' ||
          typeof response.avgClusterCoefLargeWCC?.value !== 'number' ||
          typeof response.avgClusterCoefLargeWCC?.execution_time_ms !== 'number'
        ) {
          throw new Error('Некорректный формат ответа от API');
        }
        setData(response);
      } catch (err) {
        setError(`Ошибка получения данных кластеризации: ${(err as Error).message}`);
      } finally {
        setLoading(false);
      }

      // Моковые данные для тестирования

      // setData({
      //   trianglesCount: { value: 150, execution_time_ms: 1200 },
      //   globalClusteringCoef: { value: 0.25, execution_time_ms: 800 },
      //   avgClusteringCoef: { value: 0.3, execution_time_ms: 900 },
      //   avgClusterCoefLargeWCC: { value: 0.28, execution_time_ms: 1000 },
      // });
      setLoading(false);

    };
    fetchData();
  }, [datasetname]);

  const barChartData = {
    labels: ['Глобальный', 'Средний', 'Средний (WCC)'],
    datasets: [
      {
        label: 'Коэффициенты кластеризации',
        data: data
          ? [
            data.globalClusteringCoef.value,
            data.avgClusteringCoef.value,
            data.avgClusterCoefLargeWCC.value,
          ]
          : [0, 0, 0],
        backgroundColor: ['#1976d2', '#f57c00', '#388e3c'],
        borderColor: ['#1565c0', '#ef6c00', '#2e7d32'],
        borderWidth: 1,
      },
    ],
  };

  const barChartOptions = {
    scales: {
      y: { beginAtZero: true, max: 1, title: { display: true, text: 'Коэффициент' } },
      x: { title: { display: true, text: 'Тип коэффициента' } },
    },
    plugins: { legend: { display: false } },
  };

  const timeChartData = {
    labels: ['Треугольники', 'Глобальный', 'Средний', 'Средний (WCC)'],
    datasets: [
      {
        label: 'Время выполнения (мс)',
        data: data
          ? [
            data.trianglesCount.execution_time_ms,
            data.globalClusteringCoef.execution_time_ms,
            data.avgClusteringCoef.execution_time_ms,
            data.avgClusterCoefLargeWCC.execution_time_ms,
          ]
          : [0, 0, 0, 0],
        backgroundColor: '#d81b60',
        borderColor: '#c2185b',
        borderWidth: 1,
      },
    ],
  };

  const timeChartOptions = {
    scales: {
      y: { beginAtZero: true, title: { display: true, text: 'Время (мс)' } },
      x: { title: { display: true, text: 'Метрика' } },
    },
    plugins: { legend: { display: false } },
  };

  if (!datasetname) {
    return <Alert severity="error">Dataset name is not provided</Alert>;
  }

  if (loading) {
    return <CircularProgress sx={{ mt: 3 }} />;
  }

  return (
    <Box sx={{ p: 3, display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
      <Typography variant="h6" gutterBottom sx={{ textAlign: 'center' }}>
        Кластеризация
      </Typography>
      {error && (
        <Alert severity="error" sx={{ width: '100%', maxWidth: 600, mb: 2 }}>
          {error}
        </Alert>
      )}
      {data ? (
        <Table sx={{ mt: 2, maxWidth: 600, width: '100%' }}>
          <TableHead>
            <TableRow>
              <TableCell sx={{ fontWeight: 'bold', textAlign: 'center' }}>Метрика</TableCell>
              <TableCell sx={{ fontWeight: 'bold', textAlign: 'center' }}>Значение</TableCell>
              <TableCell sx={{ fontWeight: 'bold', textAlign: 'center' }}>Время (мс)</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            <TableRow>
              <TableCell sx={{ textAlign: 'center' }}>
                <Tooltip title="Количество полных подграфов на 3 вершинах">
                  <span>Число треугольников</span>
                </Tooltip>
              </TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{data.trianglesCount.value}</TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{data.trianglesCount.execution_time_ms}</TableCell>
            </TableRow>
            <TableRow>
              <TableCell sx={{ textAlign: 'center' }}>
                <Tooltip title="Доля закрытых троек в графе">
                  <span>Глобальный коэффициент</span>
                </Tooltip>
              </TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{data.globalClusteringCoef.value.toFixed(4)}</TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{data.globalClusteringCoef.execution_time_ms}</TableCell>
            </TableRow>
            <TableRow>
              <TableCell sx={{ textAlign: 'center' }}>
                <Tooltip title="Среднее значение локальных коэффициентов кластеризации">
                  <span>Средний коэффициент</span>
                </Tooltip>
              </TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{data.avgClusteringCoef.value.toFixed(4)}</TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{data.avgClusteringCoef.execution_time_ms}</TableCell>
            </TableRow>
            <TableRow>
              <TableCell sx={{ textAlign: 'center' }}>
                <Tooltip title="Средний коэффициент для наибольшей компоненты слабой связности">
                  <span>Средний коэффициент (WCC)</span>
                </Tooltip>
              </TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{data.avgClusterCoefLargeWCC.value.toFixed(4)}</TableCell>
              <TableCell sx={{ textAlign: 'center' }}>{data.avgClusterCoefLargeWCC.execution_time_ms}</TableCell>
            </TableRow>
          </TableBody>
        </Table>
      ) : (
        <Alert severity="warning" sx={{ mt: 2, maxWidth: 600, width: '100%' }}>
          Нет данных
        </Alert>
      )}
      {data && (
        <>
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Метрика', 'Значение', 'Время (мс)'],
                ['Число треугольников', data.trianglesCount.value, data.trianglesCount.execution_time_ms],
                ['Глобальный коэффициент', data.globalClusteringCoef.value.toFixed(4), data.globalClusteringCoef.execution_time_ms],
                ['Средний коэффициент', data.avgClusteringCoef.value.toFixed(4), data.avgClusteringCoef.execution_time_ms],
                ['Средний коэффициент (WCC)', data.avgClusterCoefLargeWCC.value.toFixed(4), data.avgClusterCoefLargeWCC.execution_time_ms],
              ]
                .map((row) => row.join(','))
                .join('\n');
              const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
              saveAs(blob, `${datasetname}_clustering.csv`);
            }}
            sx={{ mt: 2 }}
          >
            Скачать CSV
          </Button>
          <Box sx={{ mt: 3, width: '100%', display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение коэффициентов кластеризации
            </Typography>
            <Box sx={{ width: '100%', maxWidth: 600, height: 300, mb: 4 }}>
              <Bar data={barChartData} options={barChartOptions} />
            </Box>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Время выполнения
            </Typography>
            <Box sx={{ width: '100%', maxWidth: 600, height: 300, mb: 4 }}>
              <Bar data={timeChartData} options={timeChartOptions} />
            </Box>

          </Box>
        </>
      )}
    </Box>
  );
};

export default ClusteringComponent;