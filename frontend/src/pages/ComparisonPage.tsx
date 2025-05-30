import React, { useState } from 'react';
import {
  Box,
  Typography,
  Button,
  Alert,
  CircularProgress,
  TextField,
  Select,
  MenuItem,
  InputLabel,
  FormControl,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  Paper,
} from '@mui/material';
import { styled } from '@mui/material/styles';
import { Link } from 'react-router-dom';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip as ChartTooltip, Legend, ResponsiveContainer } from 'recharts';
import { saveAs } from 'file-saver';
import Papa, { type ParseResult } from 'papaparse';
import { datasetsDirected, datasetsUndirected, datasetsVeryLargeGraphs } from '../constants/graph';
import { graphApi } from '../api/graphApi';
import type { ClusteringData, DegreeDistributionData, RobustnessData } from '../types/graphTypes';

const allDatasets = [...datasetsDirected, ...datasetsUndirected, ...datasetsVeryLargeGraphs];

const StyledPaper = styled(Paper)(({ theme }) => ({
  padding: theme.spacing(3),
  borderRadius: theme.spacing(2),
  background: 'linear-gradient(145deg, #ffffff, #f0f4f8)',
  boxShadow: '0 4px 20px rgba(0, 0, 0, 0.1)',
  maxWidth: 900,
  margin: 'auto',
  transition: 'transform 0.3s ease-in-out',
  '&:hover': { transform: 'translateY(-4px)' },
}));

const ComparisonPage: React.FC = () => {
  const [csvFiles, setCsvFiles] = useState<{ id: string; type: string; file: File; data: Record<string, string>[] }[]>([]);
  const [selectedDatasets, setSelectedDatasets] = useState<string[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [analysisType, setAnalysisType] = useState<'clustering' | 'degree' | 'robustness' | ''>('');

  const validateCsv = (data: Record<string, string>[], type: string): boolean => {
    const headers: Record<string, string[]> = {
      clustering: ['Метрика', 'Значение', 'Время (мс)'],
      degree: ['Метрика', 'Значение'],
      robustness: ['Удалено (%)', 'Случайное', 'По степени', 'Время (мс)'],
    };
    if (!data[0]) return false;
    return headers[type].every((header) => Object.keys(data[0]).includes(header));
  };

  const handleFileUpload = (event: React.ChangeEvent<HTMLInputElement>, type: string) => {
    if (event.target.files && type) {
      Array.from(event.target.files).forEach((file) => {
        Papa.parse(file, {
          complete: (result: ParseResult<Record<string, string>>) => {
            if (!validateCsv(result.data, type)) {
              setError(`Некорректный формат CSV для ${type} в файле ${file.name}`);
              return;
            }
            setCsvFiles((prev) => [
              ...prev,
              { id: `${type}-${file.name}-${Date.now()}`, type, file, data: result.data },
            ]);
            setError(null);
          },
          header: true,
          skipEmptyLines: true,
          error: () => setError(`Ошибка обработки CSV ${file.name}`),
        });
      });
    }
  };

  const handleDatasetSelect = (event: React.ChangeEvent<{ value: unknown }>) => {
    setSelectedDatasets(event.target.value as string[]);
  };

  const fetchAnalysis = async () => {
    if (!analysisType) {
      setError('Выберите тип анализа');
      return;
    }
    if (csvFiles.length === 0 && selectedDatasets.length === 0) {
      setError('Загрузите CSV или выберите датасеты');
      return;
    }
    try {
      setLoading(true);
      setError(null);
      const results: { dataset: string; data: any }[] = [];

      // Обработка CSV
      csvFiles.forEach(({ type, data, file }) => {
        if (type === analysisType) {
          results.push({ dataset: file.name.split('.')[0], data });
        }
      });

      // Запрос к бэкенду
      if (selectedDatasets.length > 0) {
        const response = await graphApi.getBatchAnalysis(selectedDatasets, analysisType);
        response.forEach((res: { dataset: string; data: ClusteringData | DegreeDistributionData | RobustnessData[] }) => {
          results.push({ dataset: res.dataset, data: res.data });
        });
      }

      setCsvFiles((prev) =>
        results
          .filter((r) => r.data)
          .map((r) => ({
            id: `${analysisType}-${r.dataset}-${Date.now()}`,
            type: analysisType,
            file: new File([], r.dataset),
            data: Array.isArray(r.data)
              ? r.data.map((d: any) => ({
                  'Удалено (%)': d.xPercent,
                  Случайное: d.randomFraction,
                  'По степени': d.targetedFraction,
                  'Время (мс)': d.execution_time_ms,
                }))
              : Object.entries(r.data).map(([key, value]) => ({
                  Метрика: key,
                  Значение: typeof value === 'object' ? (value as any).value : value,
                  'Время (мс)': (value as any).execution_time_ms || r.data.execution_time_ms,
                })),
          }))
          .concat(prev.filter((f) => f.type !== analysisType))
      );
    } catch (err) {
      setError(`Ошибка анализа: ${(err as Error).message}`);
    } finally {
      setLoading(false);
    }
  };

  const renderComparison = () => {
    if (csvFiles.length === 0 || !analysisType) return null;
    const filteredFiles = csvFiles.filter((f) => f.type === analysisType);

    if (analysisType === 'clustering') {
      const tableData = filteredFiles.map(({ file, data }) => ({
        dataset: file.name.split('.')[0],
        triangles: parseFloat(data.find((row) => row['Метрика'] === 'Число треугольников')?.['Значение'] || 0),
        globalCoef: parseFloat(data.find((row) => row['Метрика'] === 'Глобальный коэффициент')?.['Значение'] || 0),
        avgCoef: parseFloat(data.find((row) => row['Метрика'] === 'Средний коэффициент')?.['Значение'] || 0),
        wccCoef: parseFloat(data.find((row) => row['Метрика'] === 'Средний коэффициент (WCC)')?.['Значение'] || 0),
        time: parseFloat(data[0]?.['Время (мс)'] || 0),
      }));

      return (
        <>
          <Table sx={{ mt: 2, maxWidth: 900, width: '100%' }}>
            <TableHead>
              <TableRow>
                <TableCell>Датасет</TableCell>
                <TableCell>Треугольники</TableCell>
                <TableCell>Глоб. коэф.</TableCell>
                <TableCell>Ср. коэф.</TableCell>
                <TableCell>Ср. коэф. (WCC)</TableCell>
                <TableCell>Время (мс)</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {tableData.map((row) => (
                <TableRow key={row.dataset}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>{row.triangles}</TableCell>
                  <TableCell>{row.globalCoef.toFixed(4)}</TableCell>
                  <TableCell>{row.avgCoef.toFixed(4)}</TableCell>
                  <TableCell>{row.wccCoef.toFixed(4)}</TableCell>
                  <TableCell>{row.time}</TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
          <Box sx={{ mt: 3, width: '100%', maxWidth: 900, height: 400 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение коэффициентов кластеризации
            </Typography>
            <ResponsiveContainer>
              <LineChart data={tableData}>
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis dataKey="dataset" label={{ value: 'Датасет', position: 'bottom' }} />
                <YAxis label={{ value: 'Коэффициент', angle: -90, position: 'left' }} domain={[0, 1]} />
                <ChartTooltip formatter={(value: number) => value.toFixed(4)} />
                <Legend verticalAlign="top" />
                <Line type="monotone" dataKey="globalCoef" name="Глобальный" stroke="#1976d2" />
                <Line type="monotone" dataKey="avgCoef" name="Средний" stroke="#f57c00" />
                <Line type="monotone" dataKey="wccCoef" name="Средний (WCC)" stroke="#388e3c" />
              </LineChart>
            </ResponsiveContainer>
          </Box>
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Датасет', 'Треугольники', 'Глоб. коэф.', 'Ср. коэф.', 'Ср. коэф. (WCC)', 'Время (мс)'],
                ...tableData.map((row) => [
                  row.dataset,
                  row.triangles,
                  row.globalCoef.toFixed(4),
                  row.avgCoef.toFixed(4),
                  row.wccCoef.toFixed(4),
                  row.time,
                ]),
              ]
                .map((row) => row.join(','))
                .join('\n');
              const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
              saveAs(blob, 'clustering_comparison.csv');
            }}
            sx={{ mt: 2 }}
          >
            Скачать CSV
          </Button>
        </>
      );
    }

    if (analysisType === 'degree') {
      const tableData = filteredFiles.map(({ file, data }) => ({
        dataset: file.name.split('.')[0],
        minDegree: parseFloat(data.find((row) => row['Метрика'] === 'Минимальная степень')?.['Значение'] || 0),
        avgDegree: parseFloat(data.find((row) => row['Метрика'] === 'Средняя степень')?.['Значение'] || 0),
        maxDegree: parseFloat(data.find((row) => row['Метрика'] === 'Максимальная степень')?.['Значение'] || 0),
        time: parseFloat(data.find((row) => row['Метрика'] === 'Время выполнения (мс)')?.['Значение'] || 0),
      }));

      return (
        <>
          <Table sx={{ mt: 2, maxWidth: 900, width: '100%' }}>
            <TableHead>
              <TableRow>
                <TableCell>Датасет</TableCell>
                <TableCell>Мин. степень</TableCell>
                <TableCell>Ср. степень</TableCell>
                <TableCell>Макс. степень</TableCell>
                <TableCell>Время (мс)</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {tableData.map((row) => (
                <TableRow key={row.dataset}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>{row.minDegree}</TableCell>
                  <TableCell>{row.avgDegree.toFixed(2)}</TableCell>
                  <TableCell>{row.maxDegree}</TableCell>
                  <TableCell>{row.time}</TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
          <Box sx={{ mt: 3, width: '100%', maxWidth: 900, height: 400 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение степеней узлов
            </Typography>
            <ResponsiveContainer>
              <LineChart data={tableData}>
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis dataKey="dataset" label={{ value: 'Датасет', position: 'bottom' }} />
                <YAxis label={{ value: 'Степень', angle: -90, position: 'left' }} domain={[0, 'auto']} />
                <ChartTooltip formatter={(value: number) => value.toFixed(2)} />
                <Legend verticalAlign="top" />
                <Line type="monotone" dataKey="avgDegree" name="Средняя степень" stroke="#1976d2" />
                <Line type="monotone" dataKey="maxDegree" name="Макс. степень" stroke="#f57c00" />
              </LineChart>
            </ResponsiveContainer>
          </Box>
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Датасет', 'Мин. степень', 'Ср. степень', 'Макс. степень', 'Время (мс)'],
                ...tableData.map((row) => [
                  row.dataset,
                  row.minDegree,
                  row.avgDegree.toFixed(2),
                  row.maxDegree,
                  row.time,
                ]),
              ]
                .map((row) => row.join(','))
                .join('\n');
              const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
              saveAs(blob, 'degree_comparison.csv');
            }}
            sx={{ mt: 2 }}
          >
            Скачать CSV
          </Button>
        </>
      );
    }

    if (analysisType === 'robustness') {
      const tableData = filteredFiles.flatMap(({ file, data }) =>
        data.map((row: any) => ({
          dataset: file.name.split('.')[0],
          xPercent: parseFloat(row['Удалено (%)'] || 0),
          randomFraction: parseFloat(row['Случайное'] || 0),
          targetedFraction: parseFloat(row['По степени'] || 0),
          time: parseFloat(row['Время (мс)'] || 0),
        }))
      );

      return (
        <>
          <Table sx={{ mt: 2, maxWidth: 900, width: '100%' }}>
            <TableHead>
              <TableRow>
                <TableCell>Датасет</TableCell>
                <TableCell>Удалено (%)</TableCell>
                <TableCell>Случайное</TableCell>
                <TableCell>По степени</TableCell>
                <TableCell>Время (мс)</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {tableData.map((row, index) => (
                <TableRow key={`${row.dataset}-${row.xPercent}-${index}`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>{row.xPercent}</TableCell>
                  <TableCell>{row.randomFraction.toFixed(4)}</TableCell>
                  <TableCell>{row.targetedFraction.toFixed(4)}</TableCell>
                  <TableCell>{row.time}</TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
          <Box sx={{ mt: 3, width: '100%', maxWidth: 900, height: 400 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение устойчивости
            </Typography>
            <ResponsiveContainer>
              <LineChart data={tableData}>
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis dataKey="xPercent" label={{ value: 'Удалено (%)', position: 'bottom' }} />
                <YAxis label={{ value: 'Доля вершин', angle: -90, position: 'left' }} domain={[0, 1]} />
                <ChartTooltip formatter={(value: number) => value.toFixed(4)} />
                <Legend verticalAlign="top" />
                <Line
                  type="monotone"
                  dataKey="randomFraction"
                  name="Случайное (все датасеты)"
                  stroke="#1976d2"
                />
                <Line
                  type="monotone"
                  dataKey="targetedFraction"
                  name="По степени (все датасеты)"
                  stroke="#d81b60"
                />
              </LineChart>
            </ResponsiveContainer>
          </Box>
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Датасет', 'Удалено (%)', 'Случайное', 'По степени', 'Время (мс)'],
                ...tableData.map((row) => [
                  row.dataset,
                  row.xPercent,
                  row.randomFraction.toFixed(4),
                  row.targetedFraction.toFixed(4),
                  row.time,
                ]),
              ]
                .map((row) => row.join(','))
                .join('\n');
              const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
              saveAs(blob, 'robustness_comparison.csv');
            }}
            sx={{ mt: 2 }}
          >
            Скачать CSV
          </Button>
        </>
      );
    }

    return null;
  };

  return (
    <StyledPaper elevation={0}>
      <Box sx={{ p: 3, display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
        <Typography variant="h6" gutterBottom sx={{ textAlign: 'center', color: 'primary.main' }}>
          Сравнение датасетов
        </Typography>
        {error && <Alert severity="error" sx={{ width: '100%', maxWidth: 900, mb: 2 }}>{error}</Alert>}
        <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2, width: '100%', maxWidth: 900, mb: 3 }}>
          <FormControl fullWidth>
            <InputLabel>Тип анализа</InputLabel>
            <Select
              value={analysisType}
              onChange={(e) => setAnalysisType(e.target.value as 'clustering' | 'degree' | 'robustness' | '')}
            >
              <MenuItem value="clustering">Кластеризация</MenuItem>
              <MenuItem value="degree">Распределение степеней</MenuItem>
              <MenuItem value="robustness">Устойчивость</MenuItem>
            </Select>
          </FormControl>
          <TextField
            type="file"
            inputProps={{ accept: '.csv', multiple: true }}
            onChange={(e) => handleFileUpload(e, analysisType)}
            label={`Загрузите CSV для ${analysisType || 'выбранного типа'}`}
            InputLabelProps={{ shrink: true }}
            disabled={!analysisType}
          />
          <FormControl fullWidth>
            <InputLabel>Выберите датасеты (опционально)</InputLabel>
            <Select
              multiple
              value={selectedDatasets}
              onChange={handleDatasetSelect}
              renderValue={(selected) => (selected as string[]).join(', ')}
            >
              {allDatasets.map((dataset) => (
                <MenuItem key={dataset} value={dataset}>
                  {dataset}
                </MenuItem>
              ))}
            </Select>
          </FormControl>
          <Box sx={{ display: 'flex', gap: 2 }}>
            <Button
              variant="contained"
              onClick={fetchAnalysis}
              disabled={loading || (!csvFiles.length && !selectedDatasets.length)}
            >
              Сравнить
            </Button>
            <Button variant="outlined" component={Link} to="/">
              Назад
            </Button>
          </Box>
        </Box>
        {loading && <CircularProgress sx={{ mb: 2 }} />}
        {renderComparison()}
      </Box>
    </StyledPaper>
  );
};

export default ComparisonPage;