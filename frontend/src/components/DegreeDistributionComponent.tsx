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
  Button,
} from '@mui/material';
import {
  ScatterChart,
  Scatter,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip as ChartTooltip,
  ResponsiveContainer,
} from 'recharts';
import { saveAs } from 'file-saver';
import { graphApi } from '../api/graphApi';
import type { DegreeDistributionData } from '../types/graphTypes';

interface DegreeDistributionComponentProps {
  datasetname: string | undefined;
}

const DegreeDistributionComponent: React.FC<DegreeDistributionComponentProps> = ({ datasetname }) => {
  const [data, setData] = useState<DegreeDistributionData | null>(null);
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
        const response = await graphApi.getDegreeDistribution(datasetname);
        if (
          typeof response.minDegree !== 'number' ||
          typeof response.avgDegree !== 'number' ||
          typeof response.maxDegree !== 'number' ||
          typeof response.execution_time_ms !== 'number' ||
          typeof response.probabilityDegree !== 'object' ||
          Object.entries(response.probabilityDegree).some(
            ([deg, prob]) => isNaN(parseInt(deg)) || typeof prob !== 'number' || prob < 0
          )
        ) {
          throw new Error('Некорректный формат ответа от API');
        }
        setData(response);
      } catch (err) {
        setError(`Ошибка получения распределения степеней: ${(err as Error).message}`);
      } finally {
        setLoading(false);
      }
    };
    fetchData();
  }, [datasetname]);

  const handleDownloadMetricsCsv = () => {
    if (!data) return;
    const csvContent = [
      ['Метрика', 'Значение'],
      ['Минимальная степень', data.minDegree],
      ['Средняя степень', data.avgDegree.toFixed(2)],
      ['Максимальная степень', data.maxDegree],
      ['Время выполнения (мс)', data.execution_time_ms],
    ]
      .map(row => row.join(','))
      .join('\n');
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
    saveAs(blob, `${datasetname}_degree_metrics.csv`);
  };

  const handleDownloadDistributionCsv = () => {
    if (!data) return;
    const csvContent = [
      ['Степень', 'Вероятность'],
      ...Object.entries(data.probabilityDegree).map(([degree, probability]) => [degree, probability.toFixed(6)]),
    ]
      .map(row => row.join(','))
      .join('\n');
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
    saveAs(blob, `${datasetname}_degree_distribution.csv`);
  };

  const chartData = data
    ? Object.entries(data.probabilityDegree)
        .map(([degree, probability]) => ({
          degree: parseInt(degree),
          probability,
          logDegree: Math.log10(parseInt(degree)),
          logProbability: probability > 0 ? Math.log10(probability) : null,
        }))
        .filter((d) => d.probability > 0)
    : [];

  if (!datasetname) {
    return <Alert severity="error">Dataset name is not provided</Alert>;
  }

  if (loading) {
    return <CircularProgress sx={{ mt: 3 }} />;
  }

  return (
    <Box sx={{ p: 3, display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
      <Typography variant="h6" gutterBottom sx={{ textAlign: 'center' }}>
        Распределение степеней
      </Typography>
      {error && (
        <Alert severity="error" sx={{ width: '100%', maxWidth: 600, mb: 2 }}>
          {error}
        </Alert>
      )}
      {data ? (
        <>
          <Table sx={{ mt: 2, maxWidth: 600, width: '100%' }}>
            <TableHead>
              <TableRow>
                <TableCell sx={{ fontWeight: 'bold', textAlign: 'center' }}>Метрика</TableCell>
                <TableCell sx={{ fontWeight: 'bold', textAlign: 'center' }}>Значение</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              <TableRow>
                <TableCell sx={{ textAlign: 'center' }}>Минимальная степень</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{data.minDegree}</TableCell>
              </TableRow>
              <TableRow>
                <TableCell sx={{ textAlign: 'center' }}>Средняя степень</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{data.avgDegree.toFixed(2)}</TableCell>
              </TableRow>
              <TableRow>
                <TableCell sx={{ textAlign: 'center' }}>Максимальная степень</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{data.maxDegree}</TableCell>
              </TableRow>
              <TableRow>
                <TableCell sx={{ textAlign: 'center' }}>Время выполнения (мс)</TableCell>
                <TableCell sx={{ textAlign: 'center' }}>{data.execution_time_ms}</TableCell>
              </TableRow>
            </TableBody>
          </Table>
          <Box sx={{ display: 'flex', gap: 2, mt: 2 }}>
            <Button variant="outlined" onClick={handleDownloadMetricsCsv}>
              Скачать метрики CSV
            </Button>
            <Button variant="outlined" onClick={handleDownloadDistributionCsv}>
              Скачать распределение CSV
            </Button>
          </Box>
        </>
      ) : (
        <Alert severity="warning" sx={{ mt: 2, maxWidth: 600, width: '100%' }}>
          Нет данных
        </Alert>
      )}
      {data && (
        <Box sx={{ mt: 3, width: '100%', display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
          <Box sx={{ display: 'flex', flexDirection: { xs: 'column', md: 'row' }, gap: 3, width: '100%', maxWidth: 1200 }}>
            <Box sx={{ flex: 1, maxWidth: 600, height: 400 }}>
              <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
                Обычная шкала
              </Typography>
              <ResponsiveContainer width="100%" height="100%">
                <ScatterChart margin={{ top: 20, right: 20, bottom: 20, left: 20 }}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis
                    dataKey="degree"
                    name="Степень (k)"
                    label={{ value: 'Степень (k)', position: 'bottom' }}
                    type="number"
                  />
                  <YAxis
                    dataKey="probability"
                    name="Вероятность P(k)"
                    label={{ value: 'Вероятность P(k)', angle: -90, position: 'left' }}
                    type="number"
                    domain={[0, 'auto']}
                  />
                  <ChartTooltip formatter={(value: number) => value.toFixed(4)} />
                  <Scatter data={chartData} fill="#1976d2" shape="circle" />
                </ScatterChart>
              </ResponsiveContainer>
            </Box>
            <Box sx={{ flex: 1, maxWidth: 600, height: 400 }}>
              <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
                Log-Log шкала
              </Typography>
              <ResponsiveContainer width="100%" height="100%">
                <ScatterChart margin={{ top: 20, right: 20, bottom: 20, left: 20 }}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis
                    dataKey="logDegree"
                    name="Степень (k)"
                    label={{ value: 'log10 Степень (k)', position: 'bottom' }}
                    type="number"
                  />
                  <YAxis
                    dataKey="logProbability"
                    name="Вероятность P(k)"
                    label={{ value: 'log10 Вероятность P(k)', angle: -90, position: 'left' }}
                    type="number"
                    domain={['auto', 'auto']}
                  />
                  <ChartTooltip formatter={(value: number) => `10^${value.toFixed(2)}`} />
                  <Scatter
                    data={chartData.filter((d) => d.logProbability !== null)}
                    fill="#1976d2"
                    shape="circle"
                  />
                </ScatterChart>
              </ResponsiveContainer>
            </Box>
          </Box>
        </Box>
      )}
    </Box>
  );
};

export default DegreeDistributionComponent;