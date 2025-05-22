import React, { useState } from 'react';
import {
  Box,
  Button,
  Typography,
  TextField,
  RadioGroup,
  FormControlLabel,
  Radio,
  Paper,
  Alert,
} from '@mui/material';

interface RobustnessComponentProps {
  graphId: string;
}

interface RobustnessResult {
  x_percent: number;
  removal_method: 'random' | 'targeted';
  largest_comp_frac: number;
}

const RobustnessComponent: React.FC<RobustnessComponentProps> = ({ graphId }) => {
  const [percent, setPercent] = useState<string>('');
  const [strategy, setStrategy] = useState<'random' | 'targeted'>('random');
  const [result, setResult] = useState<RobustnessResult | null>(null);
  const [error, setError] = useState<string | null>(null);

  const handleSimulate = () => {
    const xPercent = parseFloat(percent);
    if (isNaN(xPercent) || xPercent < 0 || xPercent > 100) {
      setError('Введите процент от 0 до 100');
      return;
    }

    // POST /graphs/{graphId}/robustness
    setResult({
      x_percent: xPercent,
      removal_method: strategy,
      largest_comp_frac: 0.8,
    });
    setError(null);
  };

  return (
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        Устойчивость графа
      </Typography>
      <Box sx={{ display: 'flex', gap: 2, mb: 2 }}>
        <TextField
          label="Процент удаления (%)"
          value={percent}
          onChange={(e) => setPercent(e.target.value)}
          type="number"
          size="small"
        />
        <RadioGroup
          row
          value={strategy}
          onChange={(e) => setStrategy(e.target.value as 'random' | 'targeted')}
        >
          <FormControlLabel value="random" control={<Radio />} label="Случайно" />
          <FormControlLabel value="targeted" control={<Radio />} label="По степени" />
        </RadioGroup>
        <Button variant="contained" color="primary" onClick={handleSimulate}>
          Симулировать
        </Button>
      </Box>
      {error && <Alert severity="error">{error}</Alert>}
      {result && (
        <Typography>
          Доля вершин в наибольшей компоненте: {result.largest_comp_frac.toFixed(4)} (удалено {result.x_percent}%,
          метод: {result.removal_method})
        </Typography>
      )}
    </Paper>
  );
};

export default RobustnessComponent;