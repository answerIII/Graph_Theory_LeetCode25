import React, { useState, } from 'react';
import type { ChangeEvent } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  Button,
  TextField,
  Alert,
  Box,
  CircularProgress,
  RadioGroup,
  FormControlLabel,
  Radio,
  FormControl,
  FormLabel,
  Card,
  CardContent,
  Typography,
  Tooltip,
  Select,
  MenuItem,
  InputLabel,
  LinearProgress,
  Checkbox,
  List,
  ListItem,
  ListItemText,
} from '@mui/material';
import { useGraphWorker } from '../hooks/useGraphWorker';
import { downloadGraph } from '../utils/downloadGraph';
import { datasets, testGraph1 } from '../constants/graph';

const GraphUploader: React.FC = () => {
  const [file, setFile] = useState<File | null>(null);
  const [fileName, setFileName] = useState<string>('');
  const [directed, setDirected] = useState<string>('');
  const [isVeryLargeGraph, setIsVeryLargeGraph] = useState<boolean>(false);
  const [downloadFormat, setDownloadFormat] = useState<'json' | 'csv' | 'msgpack'>('json');
  const [localError, setLocalError] = useState<string | null>(null);
  const [successMessage, setSuccessMessage] = useState<string | null>(null);
  const [backendLoading, setBackendLoading] = useState<boolean>(false);
  const { processFile, progress, loading, error: workerError, graph, logs, setGraph } = useGraphWorker();
  const navigate = useNavigate();

  const handleFileChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    if (event.target.files) {
      const selectedFile = event.target.files[0];
      setFile(selectedFile);
      setFileName(selectedFile.name);
      setLocalError(null);
      setSuccessMessage(null);
      setDirected('');
      setIsVeryLargeGraph(selectedFile.name.includes('orkut') || selectedFile.name.includes('vk'));
      setGraph(null);
    }
  };

  const handleDirectedChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setDirected(event.target.value);
    setLocalError(null);
  };

  const handleVeryLargeGraphChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setIsVeryLargeGraph(event.target.checked);
  };

  const handleFormatChange = (event: ChangeEvent<HTMLSelectElement>) => {
    setDownloadFormat(event.target.value as 'json' | 'csv' | 'msgpack');
  };

  const handleProcess = async () => {
    if (!file) {
      setLocalError('Выберите файл');
      return;
    }
    if (directed === '') {
      setLocalError('Выберите тип графа');
      return;
    }

    setLocalError(null);
    setSuccessMessage(null);

    try {
      await processFile(file, directed, isVeryLargeGraph);
      setSuccessMessage('Граф успешно обработан!');
    } catch (err: unknown) {
      setLocalError((err as Error).message || 'Ошибка обработки файла');
    }
  };

  const handleUpload = async () => {
    if (!graph) {
      setLocalError('Сначала обработайте файл');
      return;
    }

    setBackendLoading(true);
    setLocalError(null);
    setSuccessMessage(null);

    try {
      const graphId = fileName.split('.')[0] || 'uploaded_graph';
      setSuccessMessage(`Граф ${graphId} отправлен на бэкенд!`);
    } catch (err: unknown) {
      setLocalError((err as Error).message || 'Ошибка отправки на бэкенд');
    } finally {
      setBackendLoading(false);
    }
  };

  const handleDownload = async () => {
    if (!graph) {
      setLocalError('Сначала обработайте файл');
      return;
    }

    try {
      await downloadGraph(graph, fileName || 'graph', downloadFormat);
      setSuccessMessage(`Граф скачан в формате ${downloadFormat.toUpperCase()}!`);
      setLocalError(null);
    } catch (err: unknown) {
      setLocalError((err as Error).message || `Ошибка скачивания в формате ${downloadFormat.toUpperCase()}`);
    }
  };

  const handleUseTestGraph = () => {
    setGraph(testGraph1);
    setFile(null);
    setFileName('test_graph');
    setDirected('false');
    setIsVeryLargeGraph(false);
    setLocalError(null);
    setSuccessMessage('Тестовый граф загружен!');
  };

  const handleDatasetSelect = async (datasetName: string) => {
    setBackendLoading(true);
    setLocalError(null);
    setSuccessMessage(null);

    try {
      setSuccessMessage(`Датасет ${datasetName} выбран!`);
      setFileName(datasetName);
      setIsVeryLargeGraph(datasetName.includes('orkut') || datasetName.includes('vk'));
      setGraph(null);
    } catch (err: unknown) {
      setLocalError((err as Error).message || `Ошибка обработки датасета ${datasetName}`);
    } finally {
      setBackendLoading(false);
    }
  };

  const handleAnalyze = () => {
    if (!graph && !fileName) {
      setLocalError('Сначала обработайте файл или выберите датасет');
      return;
    }
    const graphId = fileName.split('.')[0] || 'test_graph';
    navigate(`/graphs/${graphId}`);
  };

  return (
    <Card sx={{ maxWidth: 700, mx: 'auto', mt: 4, boxShadow: 3 }}>
      <CardContent>
        <Typography variant="h5" gutterBottom>
          Загрузка графа
        </Typography>
        <Typography variant="body2" color="text.secondary" sx={{ mb: 2 }}>
          Выберите файл с данными графа или датасет, укажите тип графа и формат скачивания.
          <br />
          Поддерживаемые форматы: .csv, .txt, .mtx, .msgpack
        </Typography>
        <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>
          <Box sx={{ display: 'flex', gap: 2 }}>
            <TextField
              type="file"
              inputProps={{ accept: '.csv,.txt,.mtx,.msgpack' }}
              onChange={handleFileChange}
              fullWidth
              variant="outlined"
              label="Выберите файл"
              InputLabelProps={{ shrink: true }}
              disabled={loading || backendLoading}
            />
            <Button variant="outlined" onClick={handleUseTestGraph} disabled={loading || backendLoading}>
              Тестовый граф
            </Button>
          </Box>
          <Box sx={{ display: 'flex', flexDirection: 'column', gap: 1 }}>
            <Typography variant="h6">Доступные датасеты</Typography>
            <Box sx={{ display: 'flex', gap: 1, flexWrap: 'wrap' }}>
              {datasets.map((dataset) => (
                <Button
                  key={dataset}
                  variant="outlined"
                  onClick={() => handleDatasetSelect(dataset)}
                  disabled={loading || backendLoading}
                >
                  {dataset}
                </Button>
              ))}
            </Box>
          </Box>
          <FormControl component="fieldset" error={directed === '' && localError !== null}>
            <FormLabel component="legend">
              Тип графа{' '}
              <Tooltip title="Ориентированный граф имеет направленные рёбра (A→B ≠ B→A). Неориентированный — ненаправленные (A—B = B—A).">
                <Typography component="span" color="primary" sx={{ cursor: 'help' }}>
                  (?)
                </Typography>
              </Tooltip>
            </FormLabel>
            <RadioGroup row value={directed} onChange={handleDirectedChange}>
              <FormControlLabel
                value="true"
                control={<Radio />}
                label="Ориентированный"
                disabled={(file === null && fileName === '') || loading || backendLoading}
              />
              <FormControlLabel
                value="false"
                control={<Radio />}
                label="Неориентированный"
                disabled={(file === null && fileName === '') || loading || backendLoading}
              />
            </RadioGroup>
          </FormControl>
          <FormControlLabel
            control={<Checkbox checked={isVeryLargeGraph} onChange={handleVeryLargeGraphChange} />}
            label="Режим very_large_graphs (добавляет обратные рёбра для неориентированных графов)"
            disabled={(file === null && fileName === '') || loading || backendLoading}
          />
          <FormControl variant="outlined" sx={{ maxWidth: 200 }}>
            <InputLabel>Формат скачивания</InputLabel>
            <Select
              value={downloadFormat}
              onChange={handleFormatChange}
              label="Формат скачивания"
              disabled={loading || backendLoading}
            >
              <MenuItem value="json">JSON</MenuItem>
              <MenuItem value="csv">CSV</MenuItem>
              <MenuItem value="msgpack">MessagePack</MenuItem>
            </Select>
          </FormControl>
          <Box sx={{ display: 'flex', gap: 2 }}>
            <Button
              variant="contained"
              onClick={handleProcess}
              disabled={!file || directed === '' || loading || backendLoading}
              startIcon={(loading || backendLoading) ? <CircularProgress size={20} /> : null}
            >
              Обработать
            </Button>
            <Button
              variant="outlined"
              onClick={handleUpload}
              disabled={!graph || loading || backendLoading}
              startIcon={(loading || backendLoading) ? <CircularProgress size={20} /> : null}
            >
              Отправить
            </Button>
            <Button
              variant="outlined"
              color="secondary"
              onClick={handleDownload}
              disabled={!graph || loading || backendLoading}
            >
              Скачать
            </Button>
            <Button
              variant="contained"
              color="primary"
              onClick={handleAnalyze}
              disabled={(!graph && !fileName) || loading || backendLoading}
            >
              Перейти к анализу
            </Button>
          </Box>
          {(loading || backendLoading) && (
            <Box sx={{ mt: 2 }}>
              <Typography>Обработка: {progress}%</Typography>
              <LinearProgress variant="determinate" value={progress} />
            </Box>
          )}
          {graph && (
            <Alert severity="info" sx={{ mt: 2 }}>
              <Typography>
                Вершин: {graph.numNodes}
                <br />
                Рёбер: {graph.edgeCount}
                <br />
                Максимальный ID вершины: {graph.maxVertexId}
                <br />
                Тип: {graph.is_directed ? 'Ориентированный' : 'Неориентированный'}
              </Typography>
            </Alert>
          )}
          {logs.length > 0 && (
            <Box sx={{ mt: 2, maxHeight: 200, overflowY: 'auto' }}>
              <Typography variant="h6">Логи обработки</Typography>
              <List dense>
                {logs.map((log, index) => (
                  <ListItem key={index}>
                    <ListItemText primary={log} />
                  </ListItem>
                ))}
              </List>
            </Box>
          )}
          {successMessage && <Alert severity="success" sx={{ mt: 2 }}>{successMessage}</Alert>}
          {localError && <Alert severity="error" sx={{ mt: 2 }}>{localError}</Alert>}
          {workerError && <Alert severity="error" sx={{ mt: 2 }}>{workerError}</Alert>}
        </Box>
      </CardContent>
    </Card>
  );
};

export default GraphUploader;