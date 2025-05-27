import React, { useEffect, useState } from 'react';
import { Table, TableBody, TableCell, TableHead, TableRow, Typography, CircularProgress, Alert, Box } from '@mui/material';

interface ClusteringComponentProps {
  graphId: string;
}

interface ClusteringData {
  trianglesCount: number;
  globalClusteringCoef: number;
  avgClusteringCoef: number;
  avgClusterCoefLargeWCC?: number;
}

const ClusteringComponent: React.FC<ClusteringComponentProps> = ({ graphId }) => {
  const [data, setData] = useState<ClusteringData | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    // GET /graphs/{graphId}/clustering_overview
    const fetchClustering = async () => {
      // try {
      //   setLoading(true);
      //   const response = await fetch(`/api/graphs/${graphId}/clustering`);
      //   if (!response.ok) throw new Error('Ошибка сервера');
      //   const data: ClusteringData = await response.json();
      //   setData(data);
      // } catch (err) {
      //   setError((err as Error).message);
      // } finally {
      //   setLoading(false);
      // }
      setData({
      trianglesCount: 150,
      globalClusteringCoef: 0.25,
      avgClusteringCoef: 0.3,
    });
    };
    fetchClustering();
    
  }, [graphId]);

  if (loading) return <CircularProgress />;
  if (error) return <Alert severity="error">{error}</Alert>;
  if (!data) return <Typography>Загрузка...</Typography>;

  return (
    <Box sx={{ p: 2 }}>
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
            <TableCell>{data.trianglesCount}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Глобальный коэффициент кластеризации</TableCell>
            <TableCell>{data.globalClusteringCoef.toFixed(4)}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Средний коэффициент кластеризации</TableCell>
            <TableCell>{data.avgClusteringCoef.toFixed(4)}</TableCell>
          </TableRow>
        </TableBody>
      </Table>
    </Box>
  );
};

export default ClusteringComponent;