import React, { useState } from 'react';
import {
  Box,
  Typography,
  Button,
  Alert,
  CircularProgress,
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
import { Bar } from 'react-chartjs-2';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  BarElement,
  Title,
  Tooltip as ChartTooltip,
  Legend,
} from 'chart.js';
import {
  ScatterChart,
  Scatter,
  XAxis,
  YAxis,
  CartesianGrid,
  ResponsiveContainer,
  Tooltip as RechartsTooltip,
} from 'recharts';
import { LineChart, Line, Legend as RechartsLegend } from 'recharts';
import { saveAs } from 'file-saver';
import Papa, { type ParseResult } from 'papaparse';
import { datasetsDirected, datasetsUndirected, datasetsVeryLargeGraphs } from '../constants/graph';
import { graphApi } from '../api/graphApi';
import type {
  GeneralPropertiesData,
  DistanceResultEstimation,
  DegreeDistributionData,
  RobustnessData,
  DistanceResultAnalysis,
  ClusteringData,
} from '../types/graphTypes';

ChartJS.register(CategoryScale, LinearScale, BarElement, Title, ChartTooltip, Legend);

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

// Опции и цвета для графиков
const chartColors = [
  { background: '#1976d2', border: '#1565c0' },
  { background: '#d32f2f', border: '#b71c1c' },
  { background: '#388e3c', border: '#2e7d32' },
  { background: '#f57c00', border: '#ef6c00' },
  { background: '#0288d1', border: '#0277bd' },
  { background: '#7b1fa2', border: '#6a1b9a' },
];

const getChartOptions = (yTitle: string, xTitle: string) => ({
  scales: {
    y: { beginAtZero: true, title: { display: true, text: yTitle } },
    x: { title: { display: true, text: xTitle } },
  },
  plugins: {
    legend: { display: true, position: 'bottom' as const },
  },
});

const ComparisonPage: React.FC = () => {
  const [csvFiles, setCsvFiles] = useState<
    { id: string; type: string; file: File; data: Record<string, string>[] }[]
  >([]);
  const [selectedDatasets, setSelectedDatasets] = useState<string[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [analysisType, setAnalysisType] = useState<
    | 'general'
    | 'distance_estimation'
    | 'degree'
    | 'degree_distribution'
    | 'robustness'
    | 'distance_analysis'
    | 'clustering'
    | ''
  >('');

  const validateCsv = (data: Record<string, string>[], type: string): boolean => {
    const headers: Record<string, string[]> = {
      general: ['Характеристика', 'Значение'],
      distance_estimation: ['Метод', 'Диаметр', '90-й процентиль', 'Среднее расстояние', 'Время (мс)'],
      degree: ['Метрика', 'Значение'],
      degree_distribution: ['Степень', 'Вероятность'],
      robustness: ['Удалено (%)', 'Случайное', 'По степени', 'Время (мс)'],
      distance_analysis: ['Алгоритм', 'Расстояние', 'Время (мс)', 'Ориентиры', 'Начальная вершина', 'Конечная вершина'],
      clustering: ['Метрика', 'Значение', 'Время (мс)'],
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

  const handleDatasetSelect = (event: any) => {
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
        if (type === analysisType || (type === 'degree' && analysisType === 'degree_distribution')) {
          results.push({ dataset: file.name.split('.')[0], data });
        }
      });

      // Запрос к бэкенду
      if (selectedDatasets.length > 0) {
        if (analysisType === 'clustering') {
          for (const dataset of selectedDatasets) {
            const response = await graphApi.getClustering(dataset);
            results.push({ dataset, data: response });
          }
        } else {
          const response = await graphApi.getBatchAnalysis(selectedDatasets, analysisType);
          response.forEach((res: { dataset: string; data: any }) => {
            results.push({ dataset: res.dataset, data: res.data });
          });
        }
      }

      // Преобразование данных бэкенда в формат CSV
      setCsvFiles((prev) =>
        results
          .filter((r) => r.data)
          .map((r) => {
            let data: Record<string, string>[] = [];
            if (analysisType === 'general') {
              data = Object.entries(r.data).map(([key, value]) => ({
                Характеристика: key,
                Значение: typeof value === 'number' ? value.toString() : value,
              }));
            } else if (analysisType === 'distance_estimation') {
              data = (r.data as DistanceResultEstimation[]).map((d) => ({
                Метод: d.method === 'random_sample' ? 'Random Sample' : d.method === 'snowball' ? 'Snowball' : 'Random Sample & Snowball',
                Диаметр: d.diameter.toString(),
                '90-й процентиль': d.percentile90.toFixed(2),
                'Среднее расстояние': d.meanDistance.toFixed(2),
                'Время (мс)': d.execution_time_ms.toString(),
              }));
            } else if (analysisType === 'degree') {
              data = [
                { Метрика: 'Минимальная степень', Значение: r.data.minDegree.toString() },
                { Метрика: 'Средняя степень', Значение: r.data.avgDegree.toFixed(2) },
                { Метрика: 'Максимальная степень', Значение: r.data.maxDegree.toString() },
                { Метрика: 'Время выполнения (мс)', Значение: r.data.execution_time_ms.toString() },
              ];
            } else if (analysisType === 'degree_distribution') {
              data = Object.entries(r.data.probabilityDegree).map(([degree, probability]) => ({
                Степень: degree,
                Вероятность: (probability as number).toFixed(6),
              }));
            } else if (analysisType === 'robustness') {
              data = (r.data as RobustnessData[]).map((d) => ({
                'Удалено (%)': d.xPercent.toString(),
                Случайное: d.randomFraction.toFixed(4),
                'По степени': d.targetedFraction.toFixed(4),
                'Время (мс)': d.execution_time_ms.toString(),
              }));
            } else if (analysisType === 'distance_analysis') {
              data = (r.data as DistanceResultAnalysis[]).map((d) => ({
                Алгоритм: d.algorithm,
                Расстояние: d.distance?.toString() ?? 'N/A',
                'Время (мс)': d.execution_time.toString(),
                Ориентиры: d.landmarks.join(';') || '-',
                'Начальная вершина': d.start_node.toString(),
                'Конечная вершина': d.end_node.toString(),
              }));
            } else if (analysisType === 'clustering') {
              data = [
                { Метрика: 'Число треугольников', Значение: r.data.trianglesCount.value.toString(), 'Время (мс)': r.data.trianglesCount.execution_time_ms.toString() },
                { Метрика: 'Глобальный коэффициент', Значение: r.data.globalClusteringCoef.value.toFixed(4), 'Время (мс)': r.data.globalClusteringCoef.execution_time_ms.toString() },
                { Метрика: 'Средний коэффициент', Значение: r.data.avgClusteringCoef.value.toFixed(4), 'Время (мс)': r.data.avgClusteringCoef.execution_time_ms.toString() },
                { Метрика: 'Средний коэффициент (WCC)', Значение: r.data.avgClusterCoefLargeWCC.value.toFixed(4), 'Время (мс)': r.data.avgClusterCoefLargeWCC.execution_time_ms.toString() },
              ];
            }
            return {
              id: `${analysisType}-${r.dataset}-${Date.now()}`,
              type: analysisType,
              file: new File([], r.dataset),
              data,
            };
          })
          .concat(prev.filter((f) => f.type !== analysisType && !(f.type === 'degree' && analysisType === 'degree_distribution')))
      );
    } catch (err) {
      setError(`Ошибка анализа: ${(err as Error).message}`);
    } finally {
      setLoading(false);
    }
  };

  // TODO: Вынести в отдельный компонент (GeneralAnalysis)
  const renderComparison = () => {
    if (csvFiles.length === 0 || !analysisType) return null;
    const filteredFiles = csvFiles.filter((f) => f.type === analysisType || (f.type === 'degree' && analysisType === 'degree_distribution'));

    if (analysisType === 'general') {
      const tableData = filteredFiles.map(({ file, data }) => ({
        dataset: file.name.split('.')[0],
        directed: data.find((row) => row['Характеристика'] === 'Ориентированный')?.['Значение'] || 'Нет',
        nodeCount: parseInt(data.find((row) => row['Характеристика'] === 'Число вершин')?.['Значение'].replace(/,/g, '') || '0'),
        edgesCount: parseInt(data.find((row) => row['Характеристика'] === 'Число рёбер')?.['Значение'].replace(/,/g, '') || '0'),
        density: parseFloat(data.find((row) => row['Характеристика'] === 'Плотность')?.['Значение'] || '0'),
        wccCount: parseInt(data.find((row) => row['Характеристика'] === 'Компоненты слабой связности')?.['Значение'] || '0'),
        proportionWCC: parseFloat(data.find((row) => row['Характеристика'] === 'Доля вершин в max WCC')?.['Значение'] || '0'),
        sccCount: data.find((row) => row['Характеристика'] === 'Компоненты сильной связности')?.['Значение'] || 'N/A',
        proportionSCC: data.find((row) => row['Характеристика'] === 'Доля вершин в max SCC')?.['Значение'] || 'N/A',
      }));

      const nodeEdgeChartData = {
        labels: tableData.map((d) => d.dataset),
        datasets: [
          {
            label: 'Число вершин',
            data: tableData.map((d) => d.nodeCount),
            backgroundColor: chartColors[0].background,
            borderColor: chartColors[0].border,
            borderWidth: 1,
          },
          {
            label: 'Число рёбер',
            data: tableData.map((d) => d.edgesCount),
            backgroundColor: chartColors[1].background,
            borderColor: chartColors[1].border,
            borderWidth: 1,
          },
        ],
      };

      const densityChartData = {
        labels: tableData.map((d) => d.dataset),
        datasets: [
          {
            label: 'Плотность',
            data: tableData.map((d) => d.density),
            backgroundColor: chartColors[2].background,
            borderColor: chartColors[2].border,
            borderWidth: 1,
          },
        ],
      };

      return (
        <>
          <Table sx={{ mt: 2, maxWidth: 900, width: '100%' }}>
            <TableHead>
              <TableRow>
                <TableCell>Датасет</TableCell>
                <TableCell>Ориентированный</TableCell>
                <TableCell>Вершины</TableCell>
                <TableCell>Рёбра</TableCell>
                <TableCell>Плотность</TableCell>
                <TableCell>WCC</TableCell>
                <TableCell>Доля WCC</TableCell>
                <TableCell>SCC</TableCell>
                <TableCell>Доля SCC</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {tableData.map((row) => (
                <TableRow key={row.dataset}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>{row.directed}</TableCell>
                  <TableCell>{row.nodeCount.toLocaleString()}</TableCell>
                  <TableCell>{row.edgesCount.toLocaleString()}</TableCell>
                  <TableCell>{row.density.toFixed(4)}</TableCell>
                  <TableCell>{row.wccCount}</TableCell>
                  <TableCell>{row.proportionWCC.toFixed(2)}</TableCell>
                  <TableCell>{row.sccCount}</TableCell>
                  <TableCell>{row.proportionSCC}</TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Датасет', 'Ориентированный', 'Вершины', 'Рёбра', 'Плотность', 'WCC', 'Доля WCC', 'SCC', 'Доля SCC'],
                ...tableData.map((row) => [
                  row.dataset,
                  row.directed,
                  row.nodeCount,
                  row.edgesCount,
                  row.density.toFixed(4),
                  row.wccCount,
                  row.proportionWCC.toFixed(2),
                  row.sccCount,
                  row.proportionSCC,
                ]),
              ]
                .map((row) => row.join(','))
                .join('\n');
              const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
              saveAs(blob, 'general_comparison.csv');
            }}
            sx={{ mt: 2 }}
          >
            Скачать CSV
          </Button>
          <Box sx={{ mt: 3, width: '100%', maxWidth: 900 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение вершин и рёбер
            </Typography>
            <Box sx={{ height: 300, mb: 4 }}>
              <Bar data={nodeEdgeChartData} options={getChartOptions('Количество', 'Датасет')} />
            </Box>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение плотности
            </Typography>
            <Box sx={{ height: 300 }}>
              <Bar data={densityChartData} options={getChartOptions('Плотность', 'Датасет')} />
            </Box>
          </Box>
        </>
      );
    }

    // TODO: Вынести в отдельный компонент (DistanceEstimation)
    if (analysisType === 'distance_estimation') {
      const tableData = filteredFiles.map(({ file, data }) => ({
        dataset: file.name.split('.')[0],
        randomSample: {
          diameter: parseFloat(data.find((row) => row['Метод'] === 'Random Sample')?.['Диаметр'] || '0'),
          percentile90: parseFloat(data.find((row) => row['Метод'] === 'Random Sample')?.['90-й процентиль'] || '0'),
          meanDistance: parseFloat(data.find((row) => row['Метод'] === 'Random Sample')?.['Среднее расстояние'] || '0'),
          time: parseFloat(data.find((row) => row['Метод'] === 'Random Sample')?.['Время (мс)'] || '0'),
        },
        snowball: {
          diameter: parseFloat(data.find((row) => row['Метод'] === 'Snowball')?.['Диаметр'] || '0'),
          percentile90: parseFloat(data.find((row) => row['Метод'] === 'Snowball')?.['90-й процентиль'] || '0'),
          meanDistance: parseFloat(data.find((row) => row['Метод'] === 'Snowball')?.['Среднее расстояние'] || '0'),
          time: parseFloat(data.find((row) => row['Метод'] === 'Snowball')?.['Время (мс)'] || '0'),
        },
        randomSampleSnowball: {
          diameter: parseFloat(data.find((row) => row['Метод'] === 'Random Sample & Snowball')?.['Диаметр'] || '0'),
          percentile90: parseFloat(data.find((row) => row['Метод'] === 'Random Sample & Snowball')?.['90-й процентиль'] || '0'),
          meanDistance: parseFloat(data.find((row) => row['Метод'] === 'Random Sample & Snowball')?.['Среднее расстояние'] || '0'),
          time: parseFloat(data.find((row) => row['Метод'] === 'Random Sample & Snowball')?.['Время (мс)'] || '0'),
        },
      }));

      const metricsChartData = {
        labels: tableData.map((d) => d.dataset),
        datasets: [
          {
            label: 'Диаметр (Random Sample)',
            data: tableData.map((d) => d.randomSample.diameter),
            backgroundColor: chartColors[0].background,
            borderColor: chartColors[0].border,
            borderWidth: 1,
          },
          {
            label: 'Диаметр (Snowball)',
            data: tableData.map((d) => d.snowball.diameter),
            backgroundColor: chartColors[1].background,
            borderColor: chartColors[1].border,
            borderWidth: 1,
          },
          {
            label: 'Диаметр (Random Sample & Snowball)',
            data: tableData.map((d) => d.randomSampleSnowball.diameter),
            backgroundColor: chartColors[2].background,
            borderColor: chartColors[2].border,
            borderWidth: 1,
          },
        ],
      };

      const timeChartData = {
        labels: tableData.map((d) => d.dataset),
        datasets: [
          {
            label: 'Время (Random Sample, мс)',
            data: tableData.map((d) => d.randomSample.time),
            backgroundColor: chartColors[3].background,
            borderColor: chartColors[3].border,
            borderWidth: 1,
          },
          {
            label: 'Время (Snowball, мс)',
            data: tableData.map((d) => d.snowball.time),
            backgroundColor: chartColors[4].background,
            borderColor: chartColors[4].border,
            borderWidth: 1,
          },
          {
            label: 'Время (Random Sample & Snowball, мс)',
            data: tableData.map((d) => d.randomSampleSnowball.time),
            backgroundColor: chartColors[5].background,
            borderColor: chartColors[5].border,
            borderWidth: 1,
          },
        ],
      };

      return (
        <>
          <Table sx={{ mt: 2, maxWidth: 900, width: '100%' }}>
            <TableHead>
              <TableRow>
                <TableCell>Датасет</TableCell>
                <TableCell>Метод</TableCell>
                <TableCell>Диаметр</TableCell>
                <TableCell>90-й процентиль</TableCell>
                <TableCell>Среднее расстояние</TableCell>
                <TableCell>Время (мс)</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {tableData.map((row) => [
                <TableRow key={`${row.dataset}-randomSample`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>Random Sample</TableCell>
                  <TableCell>{row.randomSample.diameter}</TableCell>
                  <TableCell>{row.randomSample.percentile90.toFixed(2)}</TableCell>
                  <TableCell>{row.randomSample.meanDistance.toFixed(2)}</TableCell>
                  <TableCell>{row.randomSample.time}</TableCell>
                </TableRow>,
                <TableRow key={`${row.dataset}-snowball`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>Snowball</TableCell>
                  <TableCell>{row.snowball.diameter}</TableCell>
                  <TableCell>{row.snowball.percentile90.toFixed(2)}</TableCell>
                  <TableCell>{row.snowball.meanDistance.toFixed(2)}</TableCell>
                  <TableCell>{row.snowball.time}</TableCell>
                </TableRow>,
                <TableRow key={`${row.dataset}-randomSampleSnowball`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>Random Sample & Snowball</TableCell>
                  <TableCell>{row.randomSampleSnowball.diameter}</TableCell>
                  <TableCell>{row.randomSampleSnowball.percentile90.toFixed(2)}</TableCell>
                  <TableCell>{row.randomSampleSnowball.meanDistance.toFixed(2)}</TableCell>
                  <TableCell>{row.randomSampleSnowball.time}</TableCell>
                </TableRow>,
              ])}
            </TableBody>
          </Table>
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Датасет', 'Метод', 'Диаметр', '90-й процентиль', 'Среднее расстояние', 'Время (мс)'],
                ...tableData.flatMap((row) => [
                  [row.dataset, 'Random Sample', row.randomSample.diameter, row.randomSample.percentile90.toFixed(2), row.randomSample.meanDistance.toFixed(2), row.randomSample.time],
                  [row.dataset, 'Snowball', row.snowball.diameter, row.snowball.percentile90.toFixed(2), row.snowball.meanDistance.toFixed(2), row.snowball.time],
                  [row.dataset, 'Random Sample & Snowball', row.randomSampleSnowball.diameter, row.randomSampleSnowball.percentile90.toFixed(2), row.randomSampleSnowball.meanDistance.toFixed(2), row.randomSampleSnowball.time],
                ]),
              ]
                .map((row) => row.join(','))
                .join('\n');
              const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
              saveAs(blob, 'distance_estimation_comparison.csv');
            }}
            sx={{ mt: 2 }}
          >
            Скачать CSV
          </Button>
          <Box sx={{ mt: 3, width: '100%', maxWidth: 900 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение диаметров
            </Typography>
            <Box sx={{ height: 300, mb: 4 }}>
              <Bar data={metricsChartData} options={getChartOptions('Диаметр', 'Датасет')} />
            </Box>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение времени выполнения
            </Typography>
            <Box sx={{ height: 300 }}>
              <Bar data={timeChartData} options={getChartOptions('Время (мс)', 'Датасет')} />
            </Box>
          </Box>
        </>
      );
    }

    // TODO: Вынести в отдельный компонент (DegreeAnalysis)
    if (analysisType === 'degree') {
      const tableData = filteredFiles.map(({ file, data }) => ({
        dataset: file.name.split('.')[0],
        minDegree: parseFloat(data.find((row) => row['Метрика'] === 'Минимальная степень')?.['Значение'] || '0'),
        avgDegree: parseFloat(data.find((row) => row['Метрика'] === 'Средняя степень')?.['Значение'] || '0'),
        maxDegree: parseFloat(data.find((row) => row['Метрика'] === 'Максимальная степень')?.['Значение'] || '0'),
        time: parseFloat(data.find((row) => row['Метрика'] === 'Время выполнения (мс)')?.['Значение'] || '0'),
      }));

      const degreeChartData = {
        labels: tableData.map((d) => d.dataset),
        datasets: [
          {
            label: 'Минимальная степень',
            data: tableData.map((d) => d.minDegree),
            backgroundColor: chartColors[0].background,
            borderColor: chartColors[0].border,
            borderWidth: 1,
          },
          {
            label: 'Средняя степень',
            data: tableData.map((d) => d.avgDegree),
            backgroundColor: chartColors[1].background,
            borderColor: chartColors[1].border,
            borderWidth: 1,
          },
          {
            label: 'Максимальная степень',
            data: tableData.map((d) => d.maxDegree),
            backgroundColor: chartColors[2].background,
            borderColor: chartColors[2].border,
            borderWidth: 1,
          },
        ],
      };

      const timeChartData = {
        labels: tableData.map((d) => d.dataset),
        datasets: [
          {
            label: 'Время выполнения (мс)',
            data: tableData.map((d) => d.time),
            backgroundColor: chartColors[3].background,
            borderColor: chartColors[3].border,
            borderWidth: 1,
          },
        ],
      };

      return (
        <>
          <Table sx={{ mt: 2, maxWidth: 900, width: '100%' }}>
            <TableHead>
              <TableRow>
                <TableCell>Датасет</TableCell>
                <TableCell>Минимальная степень</TableCell>
                <TableCell>Средняя степень</TableCell>
                <TableCell>Максимальная степень</TableCell>
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
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Датасет', 'Минимальная степень', 'Средняя степень', 'Максимальная степень', 'Время (мс)'],
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
          <Box sx={{ mt: 3, width: '100%', maxWidth: 900 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение степеней
            </Typography>
            <Box sx={{ height: 300, mb: 4 }}>
              <Bar data={degreeChartData} options={getChartOptions('Степень', 'Датасет')} />
            </Box>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Время выполнения
            </Typography>
            <Box sx={{ height: 300 }}>
              <Bar data={timeChartData} options={getChartOptions('Время (мс)', 'Датасет')} />
            </Box>
          </Box>
        </>
      );
    }

    // TODO: Вынести в отдельный компонент (DegreeDistribution)
    if (analysisType === 'degree_distribution') {
      const chartData = filteredFiles.map(({ file, data }) => ({
        dataset: file.name.split('.')[0],
        points: data
          .map((row) => ({
            degree: parseInt(row['Степень']),
            probability: parseFloat(row['Вероятность']),
            logDegree: Math.log10(parseInt(row['Степень'])),
            logProbability: parseFloat(row['Вероятность']) > 0 ? Math.log10(parseFloat(row['Вероятность'])) : null,
          }))
          .filter((d) => d.probability > 0),
      }));

      return (
        <>
          <Box sx={{ mt: 3, width: '100%', maxWidth: 900 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Распределение степеней (Log-Log шкала)
            </Typography>
            <Box sx={{ height: 400 }}>
              <ResponsiveContainer width="100%" height="100%">
                <ScatterChart margin={{ top: 20, right: 20, bottom: 20, left: 20 }}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis
                    dataKey="logDegree"
                    name="Степень (k)"
                    label={{ value: 'log10 Степень (k)', position: 'bottom' }}
                    type="number"
                  />
                  <YAxis
                    dataKey="logProbability"
                    name="Вероятность P(k)"
                    label={{ value: 'log10 Вероятность P(k)', angle: -90, position: 'left' }}
                    type="number"
                    domain={['auto', 'auto']}
                  />
                  <RechartsTooltip formatter={(value: number) => `10^${value.toFixed(2)}`} />
                  {chartData.map((d, index) => (
                    <Scatter
                      key={d.dataset}
                      name={d.dataset}
                      data={d.points.filter((p) => p.logProbability !== null)}
                      fill={chartColors[index % chartColors.length].background}
                      shape="circle"
                    />
                  ))}
                  <RechartsLegend verticalAlign="top" height={36} />
                </ScatterChart>
              </ResponsiveContainer>
            </Box>
          </Box>
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Датасет', 'Степень', 'Вероятность'],
                ...chartData.flatMap((d) =>
                  d.points.map((p) => [d.dataset, p.degree, p.probability.toFixed(6)])
                ),
              ]
                .map((row) => row.join(','))
                .join('\n');
              const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
              saveAs(blob, 'degree_distribution_comparison.csv');
            }}
            sx={{ mt: 2 }}
          >
            Скачать CSV
          </Button>
        </>
      );
    }

    // TODO: Вынести в отдельный компонент (RobustnessAnalysis)
    if (analysisType === 'robustness') {
      const tableData = filteredFiles.map(({ file, data }) => ({
        dataset: file.name.split('.')[0],
        points: data.map((row) => ({
          xPercent: parseFloat(row['Удалено (%)']),
          randomFraction: parseFloat(row['Случайное']),
          targetedFraction: parseFloat(row['По степени']),
          time: parseFloat(row['Время (мс)']),
        })),
      }));

      const chartData = tableData.map((d) => ({
        dataset: d.dataset,
        points: d.points.map((p) => ({
          xPercent: p.xPercent,
          randomFraction: p.randomFraction,
          targetedFraction: p.targetedFraction,
        })),
      }));

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
              {tableData.map((row) =>
                row.points.map((point, index) => (
                  <TableRow key={`${row.dataset}-${index}`}>
                    <TableCell>{row.dataset}</TableCell>
                    <TableCell>{point.xPercent}</TableCell>
                    <TableCell>{point.randomFraction.toFixed(4)}</TableCell>
                    <TableCell>{point.targetedFraction.toFixed(4)}</TableCell>
                    <TableCell>{point.time}</TableCell>
                  </TableRow>
                ))
              )}
            </TableBody>
          </Table>
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Датасет', 'Удалено (%)', 'Случайное', 'По степени', 'Время (мс)'],
                ...tableData.flatMap((row) =>
                  row.points.map((p) => [
                    row.dataset,
                    p.xPercent,
                    p.randomFraction.toFixed(4),
                    p.targetedFraction.toFixed(4),
                    p.time,
                  ])
                ),
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
          <Box sx={{ mt: 3, width: '100%', maxWidth: 900 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Устойчивость графа
            </Typography>
            {chartData.map((d) => (
              <Box key={d.dataset} sx={{ height: 400, mb: 4 }}>
                <Typography variant="subtitle2" sx={{ textAlign: 'center', mb: 1 }}>
                  {d.dataset}
                </Typography>
                <ResponsiveContainer width="100%" height="100%">
                  <LineChart data={d.points} margin={{ top: 20, right: 20, bottom: 20, left: 20 }}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis
                      dataKey="xPercent"
                      name="Удалено (%)"
                      label={{ value: 'Удалено узлов (%)', position: 'bottom' }}
                    />
                    <YAxis
                      name="Доля вершин"
                      label={{ value: 'Доля вершин', angle: -90, position: 'left' }}
                      domain={[0, 1]}
                    />
                    <RechartsTooltip formatter={(value: number) => value.toFixed(4)} />
                    <RechartsLegend verticalAlign="top" height={36} />
                    <Line
                      type="monotone"
                      dataKey="randomFraction"
                      name="Случайное удаление"
                      stroke={chartColors[0].background}
                      strokeWidth={2}
                      dot={{ r: 4 }}
                    />
                    <Line
                      type="monotone"
                      dataKey="targetedFraction"
                      name="Удаление по степени"
                      stroke={chartColors[1].background}
                      strokeWidth={2}
                      dot={{ r: 4 }}
                    />
                  </LineChart>
                </ResponsiveContainer>
              </Box>
            ))}
          </Box>
        </>
      );
    }

    // TODO: Вынести в отдельный компонент (DistanceAnalysis)
    if (analysisType === 'distance_analysis') {
      const tableData = filteredFiles.map(({ file, data }) => ({
        dataset: file.name.split('.')[0],
        bfs: {
          distance: parseFloat(data.find((row) => row['Алгоритм'] === 'bfs')?.['Расстояние'] || '0'),
          time: parseFloat(data.find((row) => row['Алгоритм'] === 'bfs')?.['Время (мс)'] || '0'),
          landmarks: data.find((row) => row['Алгоритм'] === 'bfs')?.['Ориентиры'] || '-',
          start_node: parseInt(data.find((row) => row['Алгоритм'] === 'bfs')?.['Начальная вершина'] || '0'),
          end_node: parseInt(data.find((row) => row['Алгоритм'] === 'bfs')?.['Конечная вершина'] || '0'),
        },
        landmarksBasic: {
          distance: parseFloat(data.find((row) => row['Алгоритм'] === 'landmarks-basic')?.['Расстояние'] || '0'),
          time: parseFloat(data.find((row) => row['Алгоритм'] === 'landmarks-basic')?.['Время (мс)'] || '0'),
          landmarks: data.find((row) => row['Алгоритм'] === 'landmarks-basic')?.['Ориентиры'] || '-',
          start_node: parseInt(data.find((row) => row['Алгоритм'] === 'landmarks-basic')?.['Начальная вершина'] || '0'),
          end_node: parseInt(data.find((row) => row['Алгоритм'] === 'landmarks-basic')?.['Конечная вершина'] || '0'),
        },
        landmarksBfs: {
          distance: parseFloat(data.find((row) => row['Алгоритм'] === 'landmarks-bfs')?.['Расстояние'] || '0'),
          time: parseFloat(data.find((row) => row['Алгоритм'] === 'landmarks-bfs')?.['Время (мс)'] || '0'),
          landmarks: data.find((row) => row['Алгоритм'] === 'landmarks-bfs')?.['Ориентиры'] || '-',
          start_node: parseInt(data.find((row) => row['Алгоритм'] === 'landmarks-bfs')?.['Начальная вершина'] || '0'),
          end_node: parseInt(data.find((row) => row['Алгоритм'] === 'landmarks-bfs')?.['Конечная вершина'] || '0'),
        },
      }));

      const distanceChartData = {
        labels: tableData.map((d) => d.dataset),
        datasets: [
          {
            label: 'Расстояние (bfs)',
            data: tableData.map((d) => d.bfs.distance),
            backgroundColor: chartColors[0].background,
            borderColor: chartColors[0].border,
            borderWidth: 1,
          },
          {
            label: 'Расстояние (landmarks-basic)',
            data: tableData.map((d) => d.landmarksBasic.distance),
            backgroundColor: chartColors[1].background,
            borderColor: chartColors[1].border,
            borderWidth: 1,
          },
          {
            label: 'Расстояние (landmarks-bfs)',
            data: tableData.map((d) => d.landmarksBfs.distance),
            backgroundColor: chartColors[2].background,
            borderColor: chartColors[2].border,
            borderWidth: 1,
          },
        ],
      };

      const timeChartData = {
        labels: tableData.map((d) => d.dataset),
        datasets: [
          {
            label: 'Время (bfs, мс)',
            data: tableData.map((d) => d.bfs.time),
            backgroundColor: chartColors[3].background,
            borderColor: chartColors[3].border,
            borderWidth: 1,
          },
          {
            label: 'Время (landmarks-basic, мс)',
            data: tableData.map((d) => d.landmarksBasic.time),
            backgroundColor: chartColors[4].background,
            borderColor: chartColors[4].border,
            borderWidth: 1,
          },
          {
            label: 'Время (landmarks-bfs, мс)',
            data: tableData.map((d) => d.landmarksBfs.time),
            backgroundColor: chartColors[5].background,
            borderColor: chartColors[5].border,
            borderWidth: 1,
          },
        ],
      };

      return (
        <>
          <Table sx={{ mt: 2, maxWidth: 900, width: '100%' }}>
            <TableHead>
              <TableRow>
                <TableCell>Датасет</TableCell>
                <TableCell>Алгоритм</TableCell>
                <TableCell>Расстояние</TableCell>
                <TableCell>Время (мс)</TableCell>
                <TableCell>Ориентиры</TableCell>
                <TableCell>Начальная вершина</TableCell>
                <TableCell>Конечная вершина</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {tableData.map((row) => [
                <TableRow key={`${row.dataset}-bfs`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>bfs</TableCell>
                  <TableCell>{row.bfs.distance || 'N/A'}</TableCell>
                  <TableCell>{row.bfs.time}</TableCell>
                  <TableCell>{row.bfs.landmarks}</TableCell>
                  <TableCell>{row.bfs.start_node}</TableCell>
                  <TableCell>{row.bfs.end_node}</TableCell>
                </TableRow>,
                <TableRow key={`${row.dataset}-landmarksBasic`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>landmarks-basic</TableCell>
                  <TableCell>{row.landmarksBasic.distance || 'N/A'}</TableCell>
                  <TableCell>{row.landmarksBasic.time}</TableCell>
                  <TableCell>{row.landmarksBasic.landmarks}</TableCell>
                  <TableCell>{row.landmarksBasic.start_node}</TableCell>
                  <TableCell>{row.landmarksBasic.end_node}</TableCell>
                </TableRow>,
                <TableRow key={`${row.dataset}-landmarksBfs`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>landmarks-bfs</TableCell>
                  <TableCell>{row.landmarksBfs.distance || 'N/A'}</TableCell>
                  <TableCell>{row.landmarksBfs.time}</TableCell>
                  <TableCell>{row.landmarksBfs.landmarks}</TableCell>
                  <TableCell>{row.landmarksBfs.start_node}</TableCell>
                  <TableCell>{row.landmarksBfs.end_node}</TableCell>
                </TableRow>,
              ])}
            </TableBody>
          </Table>
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Датасет', 'Алгоритм', 'Расстояние', 'Время (мс)', 'Ориентиры', 'Начальная вершина', 'Конечная вершина'],
                ...tableData.flatMap((row) => [
                  [row.dataset, 'bfs', row.bfs.distance || 'N/A', row.bfs.time, row.bfs.landmarks, row.bfs.start_node, row.bfs.end_node],
                  [row.dataset, 'landmarks-basic', row.landmarksBasic.distance || 'N/A', row.landmarksBasic.time, row.landmarksBasic.landmarks, row.landmarksBasic.start_node, row.landmarksBasic.end_node],
                  [row.dataset, 'landmarks-bfs', row.landmarksBfs.distance || 'N/A', row.landmarksBfs.time, row.landmarksBfs.landmarks, row.landmarksBfs.start_node, row.landmarksBfs.end_node],
                ]),
              ]
                .map((row) => row.join(','))
                .join('\n');
              const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
              saveAs(blob, 'distance_analysis_comparison.csv');
            }}
            sx={{ mt: 2 }}
          >
            Скачать CSV
          </Button>
          <Box sx={{ mt: 3, width: '100%', maxWidth: 900 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение расстояний
            </Typography>
            <Box sx={{ height: 300, mb: 4 }}>
              <Bar data={distanceChartData} options={getChartOptions('Расстояние', 'Датасет')} />
            </Box>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение времени выполнения
            </Typography>
            <Box sx={{ height: 300 }}>
              <Bar data={timeChartData} options={getChartOptions('Время (мс)', 'Датасет')} />
            </Box>
          </Box>
        </>
      );
    }

    // TODO: Вынести в отдельный компонент (ClusteringAnalysis)
    if (analysisType === 'clustering') {
      const tableData = filteredFiles.map(({ file, data }) => ({
        dataset: file.name.split('.')[0],
        trianglesCount: parseFloat(data.find((row) => row['Метрика'] === 'Число треугольников')?.['Значение'] || '0'),
        trianglesTime: parseFloat(data.find((row) => row['Метрика'] === 'Число треугольников')?.['Время (мс)'] || '0'),
        globalClustering: parseFloat(data.find((row) => row['Метрика'] === 'Глобальный коэффициент')?.['Значение'] || '0'),
        globalClusteringTime: parseFloat(data.find((row) => row['Метрика'] === 'Глобальный коэффициент')?.['Время (мс)'] || '0'),
        avgClustering: parseFloat(data.find((row) => row['Метрика'] === 'Средний коэффициент')?.['Значение'] || '0'),
        avgClusteringTime: parseFloat(data.find((row) => row['Метрика'] === 'Средний коэффициент')?.['Время (мс)'] || '0'),
        avgClusteringWCC: parseFloat(data.find((row) => row['Метрика'] === 'Средний коэффициент (WCC)')?.['Значение'] || '0'),
        avgClusteringWCCTime: parseFloat(data.find((row) => row['Метрика'] === 'Средний коэффициент (WCC)')?.['Время (мс)'] || '0'),
      }));

      const coefChartData = {
        labels: tableData.map((d) => d.dataset),
        datasets: [
          {
            label: 'Глобальный коэффициент',
            data: tableData.map((d) => d.globalClustering),
            backgroundColor: chartColors[0].background,
            borderColor: chartColors[0].border,
            borderWidth: 1,
          },
          {
            label: 'Средний коэффициент',
            data: tableData.map((d) => d.avgClustering),
            backgroundColor: chartColors[1].background,
            borderColor: chartColors[1].border,
            borderWidth: 1,
          },
          {
            label: 'Средний коэффициент (WCC)',
            data: tableData.map((d) => d.avgClusteringWCC),
            backgroundColor: chartColors[2].background,
            borderColor: chartColors[2].border,
            borderWidth: 1,
          },
        ],
      };

      const timeChartData = {
        labels: tableData.map((d) => d.dataset),
        datasets: [
          {
            label: 'Треугольники (мс)',
            data: tableData.map((d) => d.trianglesTime),
            backgroundColor: chartColors[3].background,
            borderColor: chartColors[3].border,
            borderWidth: 1,
          },
          {
            label: 'Глобальный коэффициент (мс)',
            data: tableData.map((d) => d.globalClusteringTime),
            backgroundColor: chartColors[4].background,
            borderColor: chartColors[4].border,
            borderWidth: 1,
          },
          {
            label: 'Средний коэффициент (мс)',
            data: tableData.map((d) => d.avgClusteringTime),
            backgroundColor: chartColors[5].background,
            borderColor: chartColors[5].border,
            borderWidth: 1,
          },
          {
            label: 'Средний коэффициент WCC (мс)',
            data: tableData.map((d) => d.avgClusteringWCCTime),
            backgroundColor: chartColors[0].background,
            borderColor: chartColors[0].border,
            borderWidth: 1,
          },
        ],
      };

      return (
        <>
          <Table sx={{ mt: 2, maxWidth: 900, width: '100%' }}>
            <TableHead>
              <TableRow>
                <TableCell>Датасет</TableCell>
                <TableCell>Метрика</TableCell>
                <TableCell>Значение</TableCell>
                <TableCell>Время (мс)</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {tableData.map((row) => [
                <TableRow key={`${row.dataset}-triangles`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>Число треугольников</TableCell>
                  <TableCell>{row.trianglesCount}</TableCell>
                  <TableCell>{row.trianglesTime}</TableCell>
                </TableRow>,
                <TableRow key={`${row.dataset}-global`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>Глобальный коэффициент</TableCell>
                  <TableCell>{row.globalClustering.toFixed(4)}</TableCell>
                  <TableCell>{row.globalClusteringTime}</TableCell>
                </TableRow>,
                <TableRow key={`${row.dataset}-avg`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>Средний коэффициент</TableCell>
                  <TableCell>{row.avgClustering.toFixed(4)}</TableCell>
                  <TableCell>{row.avgClusteringTime}</TableCell>
                </TableRow>,
                <TableRow key={`${row.dataset}-avgWCC`}>
                  <TableCell>{row.dataset}</TableCell>
                  <TableCell>Средний коэффициент (WCC)</TableCell>
                  <TableCell>{row.avgClusteringWCC.toFixed(4)}</TableCell>
                  <TableCell>{row.avgClusteringWCCTime}</TableCell>
                </TableRow>,
              ])}
            </TableBody>
          </Table>
          <Button
            variant="outlined"
            onClick={() => {
              const csvContent = [
                ['Датасет', 'Метрика', 'Значение', 'Время (мс)'],
                ...tableData.flatMap((row) => [
                  [row.dataset, 'Число треугольников', row.trianglesCount, row.trianglesTime],
                  [row.dataset, 'Глобальный коэффициент', row.globalClustering.toFixed(4), row.globalClusteringTime],
                  [row.dataset, 'Средний коэффициент', row.avgClustering.toFixed(4), row.avgClusteringTime],
                  [row.dataset, 'Средний коэффициент (WCC)', row.avgClusteringWCC.toFixed(4), row.avgClusteringWCCTime],
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
          <Box sx={{ mt: 3, width: '100%', maxWidth: 900 }}>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Сравнение коэффициентов кластеризации
            </Typography>
            <Box sx={{ height: 300, mb: 4 }}>
              <Bar data={coefChartData} options={getChartOptions('Коэффициент', 'Датасет')} />
            </Box>
            <Typography variant="subtitle1" sx={{ textAlign: 'center', mb: 1 }}>
              Время выполнения
            </Typography>
            <Box sx={{ height: 300 }}>
              <Bar data={timeChartData} options={getChartOptions('Время (мс)', 'Датасет')} />
            </Box>
          </Box>
        </>
      );
    }

    return null;
  };

  return (
    <Box sx={{ p: 3, display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
      <StyledPaper>
        <Typography variant="h5" gutterBottom sx={{ textAlign: 'center' }}>
          Сравнение графов
        </Typography>
        <Typography variant="body2" color="text.secondary" sx={{ mb: 3, textAlign: 'center' }}>
          Загрузите CSV с результатами анализа или выберите датасеты для сравнения их характеристик.
        </Typography>
        {error && (
          <Alert severity="error" sx={{ mb: 2 }}>
            {error}
          </Alert>
        )}
        <FormControl fullWidth sx={{ mb: 2 }}>
          <InputLabel>Тип анализа</InputLabel>
          <Select
            value={analysisType}
            onChange={(e) =>
              setAnalysisType(
                e.target.value as
                  | 'general'
                  | 'distance_estimation'
                  | 'degree'
                  | 'degree_distribution'
                  | 'robustness'
                  | 'distance_analysis'
                  | 'clustering'
                  | ''
              )
            }
            label="Тип анализа"
          >
            <MenuItem value="">Выберите тип</MenuItem>
            <MenuItem value="general">Общие свойства</MenuItem>
            <MenuItem value="distance_estimation">Оценка расстояний</MenuItem>
            <MenuItem value="degree">Распределение степеней (метрики)</MenuItem>
            <MenuItem value="degree_distribution">Распределение степеней (график)</MenuItem>
            <MenuItem value="robustness">Устойчивость</MenuItem>
            <MenuItem value="distance_analysis">Вычисление расстояний</MenuItem>
            <MenuItem value="clustering">Кластеризация</MenuItem>
          </Select>
        </FormControl>
        {analysisType && (
          <Box sx={{ mb: 2 }}>
            <Typography variant="subtitle2" sx={{ mb: 1 }}>
              Загрузить CSV для {analysisType}
            </Typography>
            <input
              type="file"
              accept=".csv"
              multiple
              onChange={(e) => handleFileUpload(e, analysisType)}
              style={{ display: 'block', marginBottom: '16px' }}
            />
          </Box>
        )}
        <FormControl fullWidth sx={{ mb: 2 }}>
          <InputLabel>Датасеты</InputLabel>
          <Select
            multiple
            value={selectedDatasets}
            onChange={handleDatasetSelect}
            label="Датасеты"
            renderValue={(selected) => (selected as string[]).join(', ')}
          >
            {allDatasets.map((dataset) => (
              <MenuItem key={dataset} value={dataset}>
                {dataset}
              </MenuItem>
            ))}
          </Select>
        </FormControl>
        <Button
          variant="contained"
          onClick={fetchAnalysis}
          disabled={loading || !analysisType}
          sx={{ width: '100%' }}
        >
          {loading ? <CircularProgress size={24} /> : 'Сравнить'}
        </Button>
        <Box sx={{ mt: 2, textAlign: 'center' }}>
          <Link to="/" style={{ textDecoration: 'none' }}>
            <Button variant="text">Вернуться к выбору графа</Button>
          </Link>
        </Box>
      </StyledPaper>
      {renderComparison()}
    </Box>
  );
};

export default ComparisonPage;