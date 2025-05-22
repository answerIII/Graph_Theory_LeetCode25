import React, { useEffect, useState } from 'react';
import { Table, TableBody, TableCell, TableHead, TableRow, Paper, Typography } from '@mui/material';

interface ClusteringComponentProps {
  graphId: string;
}

interface ClusteringData {
  num_triangles: number;
  global_clustering_coef: number;
  avg_clustering_coef: number;
}

const ClusteringComponent: React.FC<ClusteringComponentProps> = ({ graphId }) => {
  const [data, setData] = useState<ClusteringData | null>(null);

  useEffect(() => {
    // GET /graphs/{graphId}/clustering_overview
    setData({
      num_triangles: 150,
      global_clustering_coef: 0.25,
      avg_clustering_coef: 0.3,
    });
  }, [graphId]);

  if (!data) return <Typography>Загрузка...</Typography>;

  return (
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        Кластеризация
      </Typography>
      <Table>
        <TableHead>
          <TableRow>
            <TableCell>Метрика</TableCell>
            <TableCell>Значение</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          <TableRow>
            <TableCell>Число треугольников</TableCell>
            <TableCell>{data.num_triangles}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Глобальный коэффициент кластеризации</TableCell>
            <TableCell>{data.global_clustering_coef.toFixed(4)}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Средний коэффициент кластеризации</TableCell>
            <TableCell>{data.avg_clustering_coef.toFixed(4)}</TableCell>
          </TableRow>
        </TableBody>
      </Table>
    </Paper>
  );
};

export default ClusteringComponent;