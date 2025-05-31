import React, { useState } from 'react';
import {
  Box,
  Button,
  Typography,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  Alert,
  CircularProgress,
  TextField,
} from '@mui/material';
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip as ChartTooltip,
  Legend,
  ResponsiveContainer,
} from 'recharts';
import { saveAs } from 'file-saver';
import { graphApi } from '../api/graphApi';
import type { RobustnessData } from '../types/graphTypes';

interface RobustnessComponentProps {
  datasetname: string | undefined;
}

const RobustnessComponent: React.FC<RobustnessComponentProps> = ({ datasetname }) => {
  const [xPercents, setXPercents] = useState<string>('0,10,20,30,40,50,60,70,80,90,100');
  const [data, setData] = useState<RobustnessData[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const validatePercents = (input: string): number[] => {
    const percents = input
      .split(',')
      .map((x) => parseFloat(x.trim()))
      .filter((x) => !isNaN(x) && x >= 0 && x <= 100);
    return [...new Set(percents)].sort((a, b) => a - b);
  };

  const fetchRobustness = async () => {
    if (!datasetname) {
      setError('Dataset name is not provided');
      return;
    }
    const percents = validatePercents(xPercents);
    if (percents.length === 0) {
      setError('Введите корректные проценты (0-100, через запятую)');
      return;
    }
    try {
      setLoading(true);
      setError(null);
      const response = await graphApi.getRobustness(datasetname, percents);
      if (
        !Array.isArray(response) ||
        response.some(
          (r) =>
            typeof r.xPercent !== 'number' ||
            typeof r.randomFraction !== 'number' ||
            typeof r.targetedFraction !== 'number' ||
            typeof r.execution_time_ms !== 'number' ||
            r.randomFraction < 0 ||
            r.randomFraction > 1 ||
            r.targetedFraction < 0 ||
            r.targetedFraction > 1
        )
      ) {
        throw new Error('Некорректный формат ответа от API');
      }
      setData(response);
    } catch (err) {
      setError(`Ошибка анализа устойчивости: ${(err as Error).message}`);
    } finally {
      setLoading(false);
    }
  };

  const handleDownloadCsv = () => {
    const csvContent = [
      ['Удалено (%)', 'Случайное', 'По степени', 'Время (мс)'],
      ...data.map((row) => [
        row.xPercent,
        row.randomFraction.toFixed(4),
        row.targetedFraction.toFixed(4),
        row.execution_time_ms,
      ]),
    ]
      .map(row => row.join(','))
      .join('\n');
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
    saveAs(blob, `${datasetname}_robustness.csv`);
  };

  const handleKeyPress = (e: React.KeyboardEvent) => {
    if (e.key === 'Enter') {
      fetchRobustness();
    }
  };

  if (!datasetname) {
    return <Alert severity="error">Dataset name is not provided</Alert>;
  }

  return (
    <Box sx={{ p: 3, display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
      <Typography variant="h6" gutterBottom sx={{ textAlign: 'center' }}>
        Устойчивость графа
      </Typography>
      {error && (
        <Alert severity="error" sx={{ width: '100%', maxWidth: 600, mb: 2 }}>
          {error}
        </Alert>
      )}
      <Box sx={{ display: 'flex', gap: 2, mb: 2, width: '100%', maxWidth: 600 }}>
        <TextField
          label="Проценты удаления (через запятую)"
          value={xPercents}
          onChange={(e) => setXPercents(e.target.value)}
          onKeyPress={handleKeyPress}
          fullWidth
          size="small"
        />
        <Button
          variant="contained"
          onClick={fetchRobustness}
          disabled={loading}
          sx={{ whiteSpace: 'nowrap' }}
        >
          Анализировать
        </Button>
      </Box>
      {loading && <CircularProgress sx={{ mb: 2 }} />}
      {data.length > 0 && (
        <>
          <Table sx={{ mt: 2, maxWidth: 600, width: '100%' }}>
            <TableHead>
              <TableRow>
                <TableCell sx={{ fontWeight: 'bold', textAlign: 'center' }}>
                  Удалено (%)
                </TableCell>
                <TableCell sx={{ fontWeight: 'bold', textAlign: 'center' }}>
                  Случайное
                </TableCell>
                <TableCell sx={{ fontWeight: 'bold', textAlign: 'center' }}>
                  По степени
                </TableCell>
                <TableCell sx={{ fontWeight: 'bold', textAlign: 'center' }}>
                  Время (мс)
                </TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {data.map((row) => (
                <TableRow key={row.xPercent}>
                  <TableCell sx={{ textAlign: 'center' }}>{row.xPercent}</TableCell>
                  <TableCell sx={{ textAlign: 'center' }}>
                    {row.randomFraction.toFixed(4)}
                  </TableCell>
                  <TableCell sx={{ textAlign: 'center' }}>
                    {row.targetedFraction.toFixed(4)}
                  </TableCell>
                  <TableCell sx={{ textAlign: 'center' }}>{row.execution_time_ms}</TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
          <Button variant="outlined" onClick={handleDownloadCsv} sx={{ mt: 2 }}>
            Скачать CSV
          </Button>
          <Box sx={{ mt: 3, width: '100%', maxWidth: 600, height: 400 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Доля вершин в наибольшей компоненте слабой связности
            </Typography>
            <ResponsiveContainer width="100%" height="100%">
              <LineChart data={data} margin={{ top: 20, right: 20, bottom: 20, left: 20 }}>
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis
                  dataKey="xPercent"
                  name="Удалено (%)"
                  label={{ value: 'Удалено узлов (%)', position: 'bottom' }}
                />
                <YAxis
                  name="Доля вершин"
                  label={{ value: 'Доля вершин', angle: -90, position: 'left' }}
                  domain={[0, 1]}
                />
                <ChartTooltip formatter={(value: number) => value.toFixed(4)} />
                <Legend verticalAlign="top" height={36} />
                <Line
                  type="monotone"
                  dataKey="randomFraction"
                  name="Случайное удаление"
                  stroke="#1976d2"
                  strokeWidth={2}
                  dot={{ r: 4 }}
                />
                <Line
                  type="monotone"
                  dataKey="targetedFraction"
                  name="Удаление по степени"
                  stroke="#d81b60"
                  strokeWidth={2}
                  dot={{ r: 4 }}
                />
              </LineChart>
            </ResponsiveContainer>
          </Box>
        </>
      )}
    </Box>
  );
};

export default RobustnessComponent;