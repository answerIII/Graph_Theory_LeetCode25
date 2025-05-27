import React, { useEffect, useState } from 'react';
import { Table, TableBody, TableCell, TableHead, TableRow, Paper, Typography, CircularProgress, Alert, Box, Tooltip } from '@mui/material';
import { Bar, BarChart, CartesianGrid, Legend, XAxis, YAxis } from 'recharts';

interface AlgorithmsRuntimeComponentProps {
  graphId: string;
}

interface RuntimeData {
  algorithm: string;
  params: string;
  elapsedMs: number;
  memoryUsedMb: number;
}

const AlgorithmsRuntimeComponent: React.FC<AlgorithmsRuntimeComponentProps> = ({ graphId }) => {
  const [data, setData] = useState<RuntimeData[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    // GET /graphs/{graphId}/algorithms_runtime
    const fetchRuntime = async () => {
      // try {
      //   setLoading(true);
      //   const response = await fetch(`/api/graphs/${graphId}/runtime`);
      //   if (!response.ok) throw new Error('Ошибка сервера');
      //   const data: RuntimeData[] = await response.json();
      //   setData(data);
      // } catch (err) {
      //   setError((err as Error).message);
      // } finally {
      //   setLoading(false);
      // }
      setData([
        { algorithm: 'double_sweep_bfs', params: 'seed=42', elapsedMs: 500, memoryUsedMb: 100 },
        { algorithm: 'compute_triangles', params: '', elapsedMs: 1200, memoryUsedMb: 200 },
      ]);
    };
    fetchRuntime();

  }, [graphId]);

  if (loading) return <CircularProgress />;
  if (error) return <Alert severity="error">{error}</Alert>;

  return (
    <Box>
      <Typography variant="h6" gutterBottom>
        Время выполнения алгоритмов
      </Typography>
      <Table>
        <TableHead>
          <TableRow>
            <TableCell>Алгоритм</TableCell>
            <TableCell>Параметры</TableCell>
            <TableCell>Время (мс)</TableCell>
            <TableCell>Память (МБ)</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {data.map((row, index) => (
            <TableRow key={index}>
              <TableCell>{row.algorithm}</TableCell>
              <TableCell>{row.params}</TableCell>
              <TableCell>{row.elapsedMs}</TableCell>
              <TableCell>{row.memoryUsedMb.toFixed(2)}</TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
      <Box sx={{ display: 'flex', justifyContent: 'center', margin: '20px 0'}}>
        <BarChart width={600} height={400} data={data}>
          <CartesianGrid />
          <XAxis dataKey="algorithm"/>
          <YAxis/>
          <Tooltip />
          <Legend />
          <Bar dataKey="elapsedMs" fill="#8884d8" name="Время (мс)" />
        </BarChart>

        <BarChart width={600} height={400} data={data}>
          <CartesianGrid />
          <XAxis dataKey="algorithm" />
          <YAxis />
          <Tooltip />
          <Legend />
          <Bar dataKey="memoryUsedMb" fill="#188910" name="Память (МБ)"/>
        </BarChart>
      </Box>


    </Box>
  );
};

export default AlgorithmsRuntimeComponent;