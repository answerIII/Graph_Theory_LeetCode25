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
import { Typography, Paper, FormControlLabel, Checkbox } from '@mui/material';

interface DegreeDistributionComponentProps {
  graphId: string;
}

interface DegreeData {
  degree_k: number;
  count_nodes: number;
  probability: number;
  log_k: number;
  log_prob: number;
}

const DegreeDistributionComponent: React.FC<DegreeDistributionComponentProps> = ({ graphId }) => {
  const [data, setData] = useState<DegreeData[]>([]);
  const [logScale, setLogScale] = useState(false);

  useEffect(() => {
    // GET /graphs/{graphId}/degree_distribution
    setData([
      { degree_k: 1, count_nodes: 1000, probability: 0.5, log_k: 0, log_prob: -0.301 },
      { degree_k: 2, count_nodes: 600, probability: 0.3, log_k: 0.301, log_prob: -0.523 },
      { degree_k: 3, count_nodes: 300, probability: 0.15, log_k: 0.477, log_prob: -0.824 },
      { degree_k: 4, count_nodes: 100, probability: 0.05, log_k: 0.602, log_prob: -1.301 },
    ]);
  }, [graphId]);

  return (
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        Распределение степеней
      </Typography>
      <FormControlLabel
        control={<Checkbox checked={logScale} onChange={(e) => setLogScale(e.target.checked)} />}
        label="Логарифмическая шкала"
      />
      <ResponsiveContainer width="100%" height={400}>
        <ScatterChart>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis
            dataKey={logScale ? 'log_k' : 'degree_k'}
            type="number"
            name="Степень"
            domain={logScale ? [0, 'auto'] : ['auto', 'auto']}
          />
          <YAxis
            dataKey={logScale ? 'log_prob' : 'probability'}
            type="number"
            name="Вероятность"
            domain={logScale ? ['auto', 0] : [0, 'auto']}
          />
          <Tooltip />
          <Legend />
          <Scatter name="Вершины" data={data} fill="#8884d8" />
        </ScatterChart>
      </ResponsiveContainer>
    </Paper>
  );
};

export default DegreeDistributionComponent;