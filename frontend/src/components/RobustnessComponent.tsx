import React, { useState } from 'react';
import {
  Box,
  Button,
  Typography,
  Alert,
  CircularProgress,
  MenuItem,
  Select,
  Slider,
} from '@mui/material';
import { CartesianGrid, Line, LineChart, XAxis, YAxis } from 'recharts';

interface RobustnessComponentProps {
  graphId: string;
}

interface RobustnessResult {
  xPercent: number;
  removalMethod: 'random' | 'targeted';
  largestComponentFraction: number;
}

const RobustnessComponent: React.FC<RobustnessComponentProps> = ({ graphId }) => {
  const [method, setMethod] = useState<'random' | 'targeted'>('random');
  const [xPercent, setXPercent] = useState(10);
  const [data, setData] = useState<RobustnessResult[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);


  const fetchRobustness = async () => {
    // POST /graphs/{graphId}/robustness
    // try {
    //   setLoading(true);
    //   const response = await fetch(`/api/graphs/${graphId}/robustness`, {
    //     method: 'POST',
    //     headers: { 'Content-Type': 'application/json' },
    //     body: JSON.stringify({ xPercent, removalMethod: method }),
    //   });
    //   if (!response.ok) throw new Error('Ошибка сервера');
    //   const data: RobustnessResult[] = await response.json();
    //   setData(data);
    // } catch (err) {
    //   setError((err as Error).message);
    // } finally {
    //   setLoading(false);
    // }
    const test: RobustnessResult[] = [{
      xPercent: 10,
      removalMethod: 'random',
      largestComponentFraction: 0.8,
    }];
    setData(test);
  };

  return (
    <Box>

      <Typography variant="h6" gutterBottom>
        Устойчивость графа
      </Typography>

      <Select value={method} onChange={(e) => setMethod(e.target.value as any)}>
        <MenuItem value="random">Случайное удаление</MenuItem>
        <MenuItem value="targeted">Удаление по степени</MenuItem>
      </Select>
      <Slider
        value={xPercent}
        onChange={(e, value) => setXPercent(value as number)}
        min={0}
        max={100}
        step={1}
        marks
        valueLabelDisplay="auto"
      />
      <Button onClick={fetchRobustness} disabled={loading}>Анализировать</Button>
      {loading && <CircularProgress />}
      {error && <Alert severity="error">{error}</Alert>}
      {data.length > 0 && (
        <LineChart width={600} height={300} data={data}>
          <CartesianGrid />
          <XAxis dataKey="xPercent" />
          <YAxis />
          {/* <Tooltip /> */}
          <Line dataKey="largestComponentFraction" stroke="#8884d8" />
        </LineChart>
      )}
    </Box>
  );
};

export default RobustnessComponent;