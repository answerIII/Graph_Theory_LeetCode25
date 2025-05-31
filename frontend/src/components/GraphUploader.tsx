import React, { useState } from 'react';
import type { ChangeEvent } from 'react';
import { useNavigate, Link } from 'react-router-dom';
import {
  Button,
  TextField,
  Alert,
  Box,
  RadioGroup,
  FormControlLabel,
  Radio,
  FormControl,
  FormLabel,
  Paper,
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
  Chip,
  Fade,
} from '@mui/material';
import { styled } from '@mui/material/styles';
import { useGraphWorker } from '../hooks/useGraphWorker';
import { downloadGraph } from '../utils/downloadGraph';
import { datasetsDirected, datasetsUndirected, datasetsVeryLargeGraphs, datasetsTestDi, datasetsTest, testGraph1 } from '../constants/graph';

const StyledPaper = styled(Paper)(({ theme }) => ({
  padding: theme.spacing(3),
  borderRadius: theme.spacing(2),
  background: 'linear-gradient(145deg, #ffffff, #f0f4f8)',
  boxShadow: '0 4px 20px rgba(0, 0, 0, 0.1)',
  maxWidth: 800,
  margin: 'auto',
  
}));

const StyledButton = styled(Button)(({ theme }) => ({
  borderRadius: theme.spacing(1),
  padding: theme.spacing(1, 2),
  transition: 'all 0.2s ease-in-out',
  '&:hover': {
    transform: 'scale(1.05)',
  },
}));

const GraphUploader: React.FC = () => {
  const [file, setFile] = useState<File | null>(null);
  const [fileName, setFileName] = useState<string>('');
  const [directed, setDirected] = useState<string>('');
  const [isVeryLargeGraph, setIsVeryLargeGraph] = useState<boolean>(false);
  const [removeLoops, setRemoveLoops] = useState<boolean>(false);
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
      setRemoveLoops(false);
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

  const handleRemoveLoopsChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setRemoveLoops(event.target.checked);
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
      await processFile(file, directed, isVeryLargeGraph, removeLoops);
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
    setRemoveLoops(false);
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
      setRemoveLoops(false);
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
    <Fade in timeout={500}>
      <StyledPaper elevation={0}>
        <Typography variant="h5" gutterBottom sx={{ color: 'primary.main', textAlign: 'center' }}>
          Загрузка и анализ графа
        </Typography>
        <Typography variant="body2" color="text.secondary" sx={{ mb: 3, textAlign: 'center' }}>
          Загрузите граф, выберите датасет или используйте тестовый граф для анализа.
        </Typography>
        <Box sx={{ display: 'flex', flexDirection: 'column', gap: 3 }}>
          <Box sx={{ display: 'flex', gap: 2, alignItems: 'center' }}>
            <TextField
              type="file"
              inputProps={{ accept: '.csv,.txt,.mtx,.msgpack' }}
              onChange={handleFileChange}
              fullWidth
              variant="outlined"
              label="Выберите файл графа"
              InputLabelProps={{ shrink: true }}
              disabled={loading || backendLoading}
              sx={{ bgcolor: 'background.paper', borderRadius: 1 }}
            />
            <StyledButton
              variant="contained"
              color="secondary"
              onClick={handleUseTestGraph}
              disabled={loading || backendLoading}
            >
              Тестовый граф
            </StyledButton>
          </Box>
          <Box sx={{ p: 2, bgcolor: 'grey.100', borderRadius: 2 }}>
            <Typography variant="h6" gutterBottom sx={{ color: 'text.primary' }}>
              Датасеты
            </Typography>
            <Box sx={{ mb: 2 }}>
              <Typography variant="subtitle2" color="text.secondary">
                Ориентированные
              </Typography>
              <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 1, mt: 1 }}>
                {datasetsDirected.map((dataset) => (
                  <Chip
                    key={dataset}
                    label={dataset}
                    onClick={() => handleDatasetSelect(dataset)}
                    disabled={loading || backendLoading}
                    clickable
                    color="primary"
                    variant="outlined"
                  />
                ))}
              </Box>
            </Box>
            <Box sx={{ mb: 2 }}>
              <Typography variant="subtitle2" color="text.secondary">
                Неориентированные
              </Typography>
              <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 1, mt: 1 }}>
                {datasetsUndirected.map((dataset) => (
                  <Chip
                    key={dataset}
                    label={dataset}
                    onClick={() => handleDatasetSelect(dataset)}
                    disabled={loading || backendLoading}
                    clickable
                    color="primary"
                    variant="outlined"
                  />
                ))}
              </Box>
            </Box>
            <Box>
              <Typography variant="subtitle2" color="text.secondary">
                Очень большие графы
              </Typography>
              <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 1, mt: 1 }}>
                {datasetsVeryLargeGraphs.map((dataset) => (
                  <Chip
                    key={dataset}
                    label={dataset}
                    onClick={() => handleDatasetSelect(dataset)}
                    disabled={loading || backendLoading}
                    clickable
                    color="primary"
                    variant="outlined"
                  />
                ))}
              </Box>
            </Box>
            <Box>
              <Typography variant="subtitle2" color="text.secondary">
                Ориентированные тестовые графы
              </Typography>
              <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 1, mt: 1 }}>
                {datasetsTestDi.map((dataset) => (
                  <Chip
                    key={dataset}
                    label={dataset}
                    onClick={() => handleDatasetSelect(dataset)}
                    disabled={loading || backendLoading}
                    clickable
                    color="primary"
                    variant="outlined"
                  />
                ))}
              </Box>
            </Box>
            <Box>
              <Typography variant="subtitle2" color="text.secondary">
                Тестовые графы
              </Typography>
              <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 1, mt: 1 }}>
                {datasetsTest.map((dataset) => (
                  <Chip
                    key={dataset}
                    label={dataset}
                    onClick={() => handleDatasetSelect(dataset)}
                    disabled={loading || backendLoading}
                    clickable
                    color="primary"
                    variant="outlined"
                  />
                ))}
              </Box>
            </Box>
          </Box>
          <FormControl component="fieldset">
            <FormLabel component="legend" sx={{ mb: 1, display: 'flex', alignItems: 'center', gap: 1 }}>
              Тип графа
              <Tooltip title="Ориентированный граф имеет направленные рёбра (A→B ≠ B→A). Неориентированный — ненаправленные (A—B = B—A).">
                <Typography variant="caption" color="primary" sx={{ cursor: 'pointer' }}>
                  (?)
                </Typography>
              </Tooltip>
            </FormLabel>
            <RadioGroup
              row
              value={directed}
              onChange={handleDirectedChange}
              sx={{ justifyContent: 'center' }}
            >
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
          <Box sx={{ display: 'flex', alignItems: 'center', gap: 2, margin: '0 auto' }}>
            <FormControlLabel
              control={<Checkbox checked={isVeryLargeGraph} onChange={handleVeryLargeGraphChange} />}
              label="Режим добавления обратных рёбер"
              disabled={(file === null && fileName === '') || loading || backendLoading}
            />
            <FormControlLabel
              control={<Checkbox checked={removeLoops} onChange={handleRemoveLoopsChange} />}
              label="Режим удаление петель"
              disabled={(file === null && fileName === '') || loading || backendLoading}
            />
            
          </Box>
          <FormControl sx={{ minWidth: 150, margin: '0 auto', textAlign: 'center'}}>
              <InputLabel>Формат</InputLabel>
              <Select
                value={downloadFormat}
                onChange={handleFormatChange}
                label="Формат"
                disabled={loading || backendLoading}
              >
                <MenuItem value="json">JSON</MenuItem>
                <MenuItem value="csv">CSV</MenuItem>
                <MenuItem value="msgpack">MessagePack</MenuItem>
              </Select>
            </FormControl>
          <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 2, justifyContent: 'center' }}>
            <StyledButton
              variant="contained"
              onClick={handleProcess}
              disabled={!file || directed === '' || loading || backendLoading}
            >
              Обработать
            </StyledButton>
            <StyledButton
              variant="contained"
              color="secondary"
              onClick={handleUpload}
              disabled={!graph || loading || backendLoading}
            >
              Отправить
            </StyledButton>
            <StyledButton
              variant="outlined"
              onClick={handleDownload}
              disabled={!graph || loading || backendLoading}
            >
              Скачать
            </StyledButton>
            <StyledButton
              variant="contained"
              color="primary"
              onClick={handleAnalyze}
              disabled={(!graph && !fileName) || loading || backendLoading}
            >
              Анализировать
            </StyledButton>
            <StyledButton
              variant="outlined"
              color="info"
              component={Link}
              to="/comparison"
            >
              Сравнить датасеты
            </StyledButton>
          </Box>
          {(loading || backendLoading) && (
            <Box sx={{ mt: 2, width: '100%' }}>
              <Typography variant="body2" color="text.secondary">
                Прогресс: {progress}%
              </Typography>
              <LinearProgress variant="determinate" value={progress} color="primary" />
            </Box>
          )}
          {graph && (
            <Fade in timeout={500}>
              <Alert severity="info" sx={{ mt: 2 }}>
                <Typography variant="body2">
                  Вершин: {graph.numNodes}<br />
                  Рёбер: {graph.edgeCount}<br />
                  Макс. ID: {graph.maxVertexId}<br />
                  Тип: {graph.is_directed ? 'Ориентированный' : 'Неориентированный'}
                </Typography>
              </Alert>
            </Fade>
          )}
          {logs.length > 0 && (
            <Box sx={{ mt: 2, maxHeight: 150, overflowY: 'auto', bgcolor: 'grey.50', p: 2, borderRadius: 1 }}>
              <Typography variant="subtitle2" color="text.primary">
                Логи
              </Typography>
              <List dense>
                {logs.map((log, index) => (
                  <ListItem key={index}>
                    <ListItemText primary={log} />
                  </ListItem>
                ))}
              </List>
            </Box>
          )}
          {successMessage && (
            <Fade in timeout={500}>
              <Alert severity="success" sx={{ mt: 2 }}>{successMessage}</Alert>
            </Fade>
          )}
          {localError && (
            <Fade in timeout={500}>
              <Alert severity="error" sx={{ mt: 2 }}>{localError}</Alert>
            </Fade>
          )}
          {workerError && (
            <Fade in timeout={500}>
              <Alert severity="error" sx={{ mt: 2 }}>{workerError}</Alert>
            </Fade>
          )}
        </Box>
      </StyledPaper>
    </Fade>
  );
};

export default GraphUploader;