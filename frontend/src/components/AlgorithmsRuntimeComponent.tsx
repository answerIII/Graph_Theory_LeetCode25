import React, { useEffect, useState } from 'react';
import { Table, TableBody, TableCell, TableHead, TableRow, Paper, Typography } from '@mui/material';

interface AlgorithmsRuntimeComponentProps {
  graphId: string;
}

interface RuntimeData {
  algorithm: string;
  params: string;
  elapsed_ms: number;
  memory_used_mb: number;
}

const AlgorithmsRuntimeComponent: React.FC<AlgorithmsRuntimeComponentProps> = ({ graphId }) => {
  const [data, setData] = useState<RuntimeData[]>([]);

  useEffect(() => {
    // GET /graphs/{graphId}/algorithms_runtime
    setData([
      { algorithm: 'double_sweep_bfs', params: 'seed=42', elapsed_ms: 500, memory_used_mb: 100 },
      { algorithm: 'compute_triangles', params: '', elapsed_ms: 1200, memory_used_mb: 200 },
    ]);
  }, [graphId]);

  return (
    <Paper sx={{ p: 2 }}>
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
              <TableCell>{row.elapsed_ms}</TableCell>
              <TableCell>{row.memory_used_mb.toFixed(2)}</TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </Paper>
  );
};

export default AlgorithmsRuntimeComponent;