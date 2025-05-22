import React, { useState } from 'react';
import { Box, Button, Typography, Table, TableBody, TableCell, TableHead, TableRow, Paper, TextField } from '@mui/material';

interface DistanceEstimationComponentProps {
  graphId: string;
}

interface DistanceResult {
  method: 'double_sweep' | 'random_sample' | 'snowball';
  diameter_est?: number;
  mean_dist?: number;
  p90_dist?: number;
  max_dist?: number;
}

const DistanceEstimationComponent: React.FC<DistanceEstimationComponentProps> = ({ graphId }) => {
  const [method, setMethod] = useState<'double_sweep' | 'random_sample' | 'snowball'>('double_sweep');
  const [results, setResults] = useState<DistanceResult | null>(null);
  const [sampleSize, setSampleSize] = useState<string>('500');

  const handleCalculate = () => {
    // POST /graphs/{graphId}/double_sweep, /random_sample, /snowball
    // Тестовые данные
    if (method === 'double_sweep') {
      setResults({ method, diameter_est: 10 });
    } else if (method === 'random_sample') {
      setResults({ method, mean_dist: 4.5, p90_dist: 8, max_dist: 10 });
    } else {
      setResults({ method, mean_dist: 4.2, p90_dist: 7, max_dist: 9 });
    }
  };

  return (
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        Оценка расстояний
      </Typography>
      <Box sx={{ mb: 2, display: 'flex', gap: 2, alignItems: 'center' }}>
        <Button
          variant={method === 'double_sweep' ? 'contained' : 'outlined'}
          onClick={() => setMethod('double_sweep')}
        >
          Double Sweep
        </Button>
        <Button
          variant={method === 'random_sample' ? 'contained' : 'outlined'}
          onClick={() => setMethod('random_sample')}
        >
          Random Sample
        </Button>
        <Button
          variant={method === 'snowball' ? 'contained' : 'outlined'}
          onClick={() => setMethod('snowball')}
        >
          Snowball
        </Button>
        {(method === 'random_sample' || method === 'snowball') && (
          <TextField
            label="Размер выборки"
            value={sampleSize}
            onChange={(e) => setSampleSize(e.target.value)}
            type="number"
            size="small"
            sx={{ width: 120 }}
          />
        )}
        <Button variant="contained" color="primary" onClick={handleCalculate}>
          Рассчитать
        </Button>
      </Box>
      {results && (
        <Table>
          <TableHead>
            <TableRow>
              <TableCell>Метрика</TableCell>
              <TableCell>Значение</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {results.method === 'double_sweep' && (
              <TableRow>
                <TableCell>Оценка диаметра</TableCell>
                <TableCell>{results.diameter_est}</TableCell>
              </TableRow>
            )}
            {(results.method === 'random_sample' || results.method === 'snowball') && (
              <>
                <TableRow>
                  <TableCell>Среднее расстояние</TableCell>
                  <TableCell>{results.mean_dist?.toFixed(2)}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell>90-й процентиль</TableCell>
                  <TableCell>{results.p90_dist}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell>Максимальное расстояние</TableCell>
                  <TableCell>{results.max_dist}</TableCell>
                </TableRow>
              </>
            )}
          </TableBody>
        </Table>
      )}
    </Paper>
  );
};

export default DistanceEstimationComponent;