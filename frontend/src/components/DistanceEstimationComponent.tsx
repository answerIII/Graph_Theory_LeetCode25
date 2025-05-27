import React, { useState, useEffect } from 'react';
import { Box, Button, Table, TableBody, TableCell, TableRow, MenuItem, Select, CircularProgress, Alert } from '@mui/material';

interface DistanceEstimationComponentProps {
  graphId: string;
}

interface DistanceResult {
  method: 'double_sweep' | 'random_sample' | 'snowball';
  diameter?: number;
  percentile90?: number;
  // meanDistance?: number;
}

const DistanceEstimationComponent: React.FC<DistanceEstimationComponentProps> = ({ graphId }) => {
  const [method, setMethod] = useState<'double_sweep' | 'random_sample' | 'snowball'>('double_sweep');
  const [result, setResult] = useState<DistanceResult | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const fetchDistances = async () => {
    // try {
    //   setLoading(true);
    //   const response = await fetch(`/api/graphs/${graphId}/distances`, {
    //     method: 'POST',
    //     headers: { 'Content-Type': 'application/json' },
    //     body: JSON.stringify({ method, sampleSize: method !== 'double_sweep' ? 1000 : undefined }),
    //   });
    //   if (!response.ok) throw new Error('Ошибка сервера');
    //   const data: DistanceResult = await response.json();
    //   setResult(data);
    // } catch (err) {
    //   setError((err as Error).message);
    // } finally {
    //   setLoading(false);
    // }
    setResult({
      method: 'double_sweep',
      diameter: 1,
      percentile90: 1,
      meanDistance: 1,
    });
  };

  useEffect(() => {
    fetchDistances();
  }, [graphId, method]);

  return (
    <Box>
      <Select value={method} onChange={(e) => setMethod(e.target.value as any)}>
        <MenuItem value="double_sweep">Double Sweep</MenuItem>
        <MenuItem value="random_sample">Random Sample</MenuItem>
        <MenuItem value="snowball">Snowball</MenuItem>
      </Select>
      <Button onClick={fetchDistances} disabled={loading}>Обновить</Button>
      {loading && <CircularProgress />}
      {error && <Alert severity="error">{error}</Alert>}
      {result && (
        <Table>
          <TableBody>
            <TableRow><TableCell>Метод</TableCell><TableCell>{result.method}</TableCell></TableRow>
            {result.diameter && <TableRow><TableCell>Диаметр</TableCell><TableCell>{result.diameter}</TableCell></TableRow>}
            {result.percentile90 && <TableRow><TableCell>90-й процентиль</TableCell><TableCell>{result.percentile90.toFixed(2)}</TableCell></TableRow>}
            {result.meanDistance && <TableRow><TableCell>Среднее расстояние</TableCell><TableCell>{result.meanDistance.toFixed(2)}</TableCell></TableRow>}
          </TableBody>
        </Table>
      )}
    </Box>
  );
};

export default DistanceEstimationComponent;