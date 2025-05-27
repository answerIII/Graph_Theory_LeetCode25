import React, { useEffect, useState } from 'react';
import {
  ScatterChart,
  Scatter,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
} from 'recharts';
import { Typography, Box, Tabs, Tab } from '@mui/material';

interface DegreeDistributionComponentProps {
  graphId: string;
}

interface DegreeData {
  minDegree: number;
  avgDegree: number;
  maxDegree: number;
  probabilityDegree: {
    degree: number;
  };
}

const DegreeDistributionComponent: React.FC<DegreeDistributionComponentProps> = ({ graphId }) => {
  const [data, setData] = useState<DegreeData[]>([]);
  const [tab, setTab] = useState(0);
  // const [loading, setLoading] = useState(false);
  // const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    // GET /graphs/{graphId}/degree_distribution
    const fetchDegrees = async () => {
      // try {
      //   setLoading(true);
      //   const response = await fetch(`/api/graphs/${graphId}/degree-distribution`);
      //   if (!response.ok) throw new Error('Ошибка сервера');
      //   const data: DegreeData[] = await response.json();
      //   setData(data);
      // } catch (err) {
      //   setError((err as Error).message);
      // } finally {
      //   setLoading(false);
      // }
    //   setData({
    //     "minDegree": 1,
    //     "avgDegree": 3.0,
    //     "maxDegree": 5,
    //     "probabilityDegree": {
    //       "1": 0.2,
    //       "2": 0.3,
    //       "3": 0.3,
    //       "4": 0.1,
    //       "5": 0.1
    // });
    };
    fetchDegrees();

  }, [graphId]);

  return (
    <Box>
      <Typography variant="h6" gutterBottom>
        Распределение степеней
      </Typography>

      <Tabs value={tab} onChange={(e, newValue) => setTab(newValue)}>
        <Tab label="Обычная шкала" />
        <Tab label="Log-Log шкала" />
      </Tabs>
      <ResponsiveContainer width="100%" height={400}>
        <ScatterChart>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey={tab === 0 ? 'degree' : 'logDegree'} name="Степень" />
          <YAxis dataKey={tab === 0 ? 'probability' : 'logProbability'} name="Вероятность" />
          <Tooltip />
          <Legend />
          <Scatter data={data} fill="#8884d8" />
        </ScatterChart>
      </ResponsiveContainer>
    </Box>
  );
};

export default DegreeDistributionComponent;