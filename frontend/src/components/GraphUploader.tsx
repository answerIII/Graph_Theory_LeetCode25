import React, { useState } from "react";
import {
    Button,
    TextField,
    Alert,
    Box,
    CircularProgress,
    Radio,
    RadioGroup,
    FormControlLabel,
    FormControl,
    FormLabel,
    Card,
    CardContent,
    Typography,
    Tooltip,
    Select,
    MenuItem,
    InputLabel,
    type SelectChangeEvent,
} from '@mui/material';
import { uploadGraph } from "../api/graphApi";
import GraphAnalyzer from './GraphAnalyzer';
import type { Graph, Edge } from "../types/graph";


// Тестовый граф
const testGraph: Graph = {
  directed: false,
  numNodes: 10,
  numEdges: 15,
  edges: [
    { from: 0, to: 1 },
    { from: 0, to: 2 },
    { from: 1, to: 2 },
    { from: 1, to: 3 },
    { from: 2, to: 3 },
    { from: 3, to: 4 },
    { from: 4, to: 5 },
    { from: 5, to: 6 },
    { from: 6, to: 7 },
    { from: 7, to: 8 },
    { from: 8, to: 9 },
    { from: 9, to: 0 },
    { from: 2, to: 4 },
    { from: 4, to: 6 },
    { from: 6, to: 8 },
  ],
};

const GraphUploader: React.FC = () => {
    const [file, setFile] = useState<File | null>(null);
    const [fileName, setFileName] = useState<string>('');
    const [error, setError] = useState<string | null>(null);
    const [loading, setLoading] = useState(false);
    const [graphInfo, setGraphInfo] = useState<Graph | null>(null);
    const [directed, setDirected] = useState<string>('');
    const [downloadFormat, setDownloadFormat] = useState<'json' | 'csv'>('json');

    const handleFileChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        if (event.target.files) {
            const selectedFile = event.target.files[0];
            setFile(selectedFile);
            setFileName(selectedFile.name);
            setError(null);
            setGraphInfo(null);
        }
    };

    const handleDirectedChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        setDirected(event.target.value);
        setGraphInfo(null);
    };

    const handleFormatChange = (event: SelectChangeEvent) => {
        setDownloadFormat(event.target.value as 'json' | 'csv');
    };

    const processFile = async (file: File): Promise<Graph> => {

        if (directed === '') {
            throw new Error('Пожалуйста, выберите тип графа: ориентированный или неориентированный');
        }

        const text = await file.text();
        const lines = text.split('\n').map(line => line.trim());
        const edges: Edge[] = [];

        // Определние формата файла
        const isCSV = file.name.endsWith('csv');
        const isMTX = file.name.endsWith('mtx');
        const isTXT = file.name.endsWith('txt');

        let headerSkiped = false;
        for (const line of lines) {

            // Пропускаем комментарии и пустые строки
            if (line.startsWith('#') || line.startsWith('%') || line === '' || line.startsWith('i') || line.startsWith('u')) continue;

            let from: number, to: number;

            if (isCSV) {

                const parts = line.split(',');

                if (parts.length < 2) continue;

                from = parseInt(parts[0]);
                to = parseInt(parts[1]);
            }
            else if (isMTX) {

                if (!headerSkiped) {
                    headerSkiped = true;
                    continue;
                }

                if (line.includes(' ')) {
                    const parts = line.split(/\s+/);

                    if (parts.length < 2) continue;

                    from = parseInt(parts[0]);
                    to = parseInt(parts[1]);
                }
                else {
                    continue; // пропуск заголовка
                }
            }
            else if (isTXT) {
                const parts = line.split(/\s+/);

                if (parts.length < 2) continue;

                from = parseInt(parts[0]);
                to = parseInt(parts[1]);
            }
            else {
                throw new Error(`Неподдерживаемый формат файла. Используйте .csv, .txt или .mtx`);
            }

            if (isNaN(from) || isNaN(to)) {
                throw new Error(`Некорректное ребро: ${line}`);
            }

            edges.push({ from, to });
        }

        // Подсчёт вершин
        const nodes = new Set<number>();
        edges.forEach(e => {
            nodes.add(e.from);
            nodes.add(e.to);
        });

        return {
            directed: directed === 'true',
            numNodes: nodes.size,
            numEdges: edges.length,
            edges,
        };
    };


    const handleProcess = async () => {
        if (!file) {
            setError('Выберите файл');
            return;
        }
        if (directed === '') {
            setError('Пожалуйста, выебрите тип графа');
            return;
        }

        setLoading(true);
        setError(null);

        try {
            const graph = await processFile(file);
            setGraphInfo(graph);
        }
        catch (err: unknown) {
            setError((err as Error).message || 'Ошибка обработки файла');
        }
        finally {
            setLoading(false);
        }
    };

    const handleUpload = async () => {
        if (!graphInfo) {
            setError('Сначала обработайте файл');
            return;
        }

        setLoading(true);
        setError(null);

        try {
            await uploadGraph(graphInfo);
            alert('Граф успешно отправлен на бэкенд!');
            setFile(null);
            setFileName('');
            setGraphInfo(null);
            setDirected('');
        }
        catch (err: unknown) {
            setError((err as Error).message || 'Ошибка отправки на бэкенд');
        }
        finally {
            setLoading(false);
        }
    };

    const handleDownload = () => {
        if (!graphInfo) {
            setError('Нет данных для скачивания');
            return;
        }

        let content: string;
        let contentType: string;
        let downloadFileName: string;

        const safeFileName = fileName || 'graph';
        const baseName = safeFileName.includes('.')
            ? fileName.split('.').slice(0, -1).join('.')
            : safeFileName;
        const processedFileName = `processed_${baseName}`;

        if (downloadFormat === 'json') {
            content = JSON.stringify(graphInfo, null, 2);
            contentType = 'application/json';
            downloadFileName = `${processedFileName}.json`;
        }
        else {
            content = [
                `directed,${graphInfo.directed}`,
                `numNodes,${graphInfo.numNodes}`,
                `numEdges,${graphInfo.numEdges}`,
                `from,to`,
                ...graphInfo.edges.map(e => `${e.from},${e.to}`),
            ].join('\n');
            contentType = 'text/csv';
            downloadFileName = `${processedFileName}.csv`;
        }

        const blob = new Blob([content], { type: contentType });
        const url = URL.createObjectURL(blob);
        const link = document.createElement('a');
        link.href = url;
        link.download = downloadFileName;
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
        URL.revokeObjectURL(url);
    };

    const handleUseTestGraph = () => {
        setGraphInfo(testGraph);
        setFile(null);
        setFileName('test_graph');
        setDirected('false');
        setError(null);
    }

    return (
        <Card sx={{ maxWidth: 700, mx: 'auto', mt: 4, boxShadow: 3 }}>
            <CardContent>
                <Typography variant="h5" gutterBottom>
                    Загрузка графа
                </Typography>

                <Typography variant="body2" color="text.secondary" sx={{ mb: 2 }}>
                    Выберите файл с данными графа и укажите, является ли он ориентированным.
                    <br />
                    Поддерживаемые форматы: .csv, .txt, .mtx
                </Typography>

                <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>

                    <Box sx={{display: 'flex', gap: 2}}>
                        <TextField
                        type="file"
                        inputProps={{ accept: '.csv, .txt, .mtx' }}
                        onChange={handleFileChange}
                        fullWidth
                        variant="outlined"
                        label="Выберите файл"
                        InputLabelProps={{ shrink: true }}
                    />
                    
                    <Button variant="outlined" onClick={handleUseTestGraph}>
                        Использовать тестовый граф
                    </Button>

                    </Box>

                    <FormControl component="fieldset" error={directed === '' && error !== null} >

                        <FormLabel component="legend">
                            Тип графа{' '}
                            <Tooltip title="Ориентированный граф имеет напрвленные рёбра (A→B ≠ B→A). Неориентированный — ненаправленные (A—B = B—A).">
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
                                disabled={file === null}
                            />
                            <FormControlLabel
                                value="false"
                                control={<Radio />}
                                label="Неориентированный"
                                disabled={file === null}
                            />
                        </RadioGroup>
                    </FormControl>

                    <FormControl variant="outlined" sx={{ maxWidth: 200 }}>

                        <InputLabel>Формат скачивания</InputLabel>

                        <Select
                            value={downloadFormat}
                            onChange={handleFormatChange}
                            label="Формат скачивания"
                        >
                            <MenuItem value="json">JSON</MenuItem>
                            <MenuItem value="csv">CSV</MenuItem>
                        </Select>
                    </FormControl>

                    <Box sx={{ display: 'flex', gap: 2 }}>
                        <Button
                            variant="contained"
                            onClick={handleProcess}
                            disabled={!file || directed === '' || loading}
                            startIcon={loading ? <CircularProgress size={20} /> : null}
                        >
                            Обработать
                        </Button>

                        <Button
                            variant="outlined"
                            onClick={handleUpload}
                            disabled={!graphInfo || loading}
                            startIcon={loading ? <CircularProgress size={20} /> : null}
                        >
                            Отправить
                        </Button>

                        <Button
                            variant="outlined"
                            color="secondary"
                            onClick={handleDownload}
                            disabled={!graphInfo || loading}
                        >
                            Скачать
                        </Button>
                    </Box>

                    {graphInfo && (
                        <Alert severity="info" sx={{ mt: 2 }}>
                            <Typography>
                                Вершин: {graphInfo.numNodes}
                                <br />
                                Рёбер: {graphInfo.numEdges}
                                <br />
                                Тип: {graphInfo.directed ? 'Ориентированный' : 'Неориентированный'}
                            </Typography>
                        </Alert>
                    )}
                    {error && (
                        <Alert severity="error" sx={{ mt: 2 }}>
                            {error}
                        </Alert>
                    )}
                    {graphInfo && <GraphAnalyzer graph={graphInfo}/>}
                </Box>
            </CardContent>
        </Card>
    );
};

export default GraphUploader;
