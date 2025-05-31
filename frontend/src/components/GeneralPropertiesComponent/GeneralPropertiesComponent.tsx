import { useEffect, useState, useRef } from 'react';
import {
  CircularProgress,
  Alert,
  Table,
  TableBody,
  TableRow,
  Box,
  Typography,
  Fade,
  Tabs,
  Tab,
  Button,
  Card,
  CardContent,
  TableHead,
  Tooltip,
} from '@mui/material';
import CytoscapeComponent from 'react-cytoscapejs';
import type { Core as Cy } from 'cytoscape';
import { saveAs } from 'file-saver';
import { graphApi } from '../../api/graphApi';
import type { GeneralPropertiesData, LogEntry } from './types';
import { findMaxWCC, findMaxSCC } from './graphAlgorithms';
import { testGraph } from './testGraph';
import { StyledTableCell, StyledTableRow, propertyDescriptions } from './styles';
import { InfoOutlined } from '@mui/icons-material';

const GeneralPropertiesComponent: React.FC<{ datasetname: string }> = ({ datasetname }) => {
  const [properties, setProperties] = useState<GeneralPropertiesData | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [tabValue, setTabValue] = useState(0);
  const [logs, setLogs] = useState<LogEntry[]>([]);
  const cyRef = useRef<Cy | null>(null);

  useEffect(() => {
    const fetchData = async () => {
      try {
        setLoading(true);
        const data = await graphApi.getGeneralProperties(datasetname);
        setProperties(data);
      } catch (err) {
        setError((err as Error).message);
      } finally {
        setLoading(false);
      }
    };
    fetchData();
  }, [datasetname]);

  const addLog = (message: string) => {
    setLogs(prev => [...prev, { message, timestamp: Date.now() }]);
  };

  const handleRunWCC = async () => {
    if (!cyRef.current) return;
    setLogs([]);
    const { maxWccSize, countWCC } = await findMaxWCC(testGraph, cyRef.current, addLog);
    addLog(`Итог: найдено ${countWCC} WCC, наибольшая содержит ${maxWccSize} вершин`);
  };

  const handleRunSCC = async () => {
    if (!cyRef.current) return;
    setLogs([]);
    const { maxSccSize, countScc } = await findMaxSCC(testGraph, cyRef.current, addLog);
    addLog(`Итог: найдено ${countScc} SCC, наибольшая содержит ${maxSccSize} вершин`);
  };

  const handleDownloadCsv = () => {
    if (!properties) return;
    const csvContent = [
      ['Характеристика', 'Значение'],
      ['Ориентированный', properties.directed ? 'Да' : 'Нет'],
      ['Число вершин', properties.nodeCount.toLocaleString()],
      ['Число рёбер', properties.edgesCount.toLocaleString()],
      ['Плотность', properties.density.toFixed(4)],
      ['Компоненты слабой связности', properties.wccCount],
      ['Доля вершин в max WCC', properties.proportionWCC.toFixed(2)],
      ...(properties.directed
        ? [
            ['Компоненты сильной связности', properties.sccCount ?? 'N/A'],
            ['Доля вершин в max SCC', properties.proportionSCC?.toFixed(2) ?? 'N/A'],
          ]
        : []),
    ]
      .map(row => row.join(','))
      .join('\n');
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8' });
    saveAs(blob, `${datasetname}_general_properties.csv`);
  };

  const graphElements = [
    ...Array.from(testGraph.adjList.keys()).map(id => ({ data: { id } })),
    ...testGraph.edges.map(edge => ({
      data: { id: `${edge.from}_${edge.to}`, source: edge.from, target: edge.to },
    })),
  ];

  if (loading) return <CircularProgress />;
  if (error) return <Alert severity="error">{error}</Alert>;
  if (!properties) return <Typography>Загрузка данных...</Typography>;

  return (
    <Fade in timeout={600}>
      <Box sx={{ p: 3, bgcolor: 'background.paper', borderRadius: 2, boxShadow: 3, maxWidth: '1200px', mx: 'auto' }}>
        <Typography variant="h5" gutterBottom color="primary.main">
          Общие свойства графа: {datasetname}
        </Typography>
        <Tabs value={tabValue} onChange={(_e, newValue) => setTabValue(newValue)} sx={{ mb: 3 }}>
          <Tab label="Свойства" />
          <Tab label="Алгоритмы" />
        </Tabs>

        {tabValue === 0 && (
          <Box >
            <Typography variant="body2" color="text.secondary" mb={3}>
              Основные метрики графа, включая топологические характеристики и структуру связности.
            </Typography>
            <Table sx={{ border: '1px solid', borderColor: 'grey.200' }}>
              <TableHead>
                <TableRow>
                  <StyledTableCell>Характеристика</StyledTableCell>
                  <StyledTableCell>Значение</StyledTableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                <StyledTableRow>
                  <StyledTableCell>
                    <Tooltip title={propertyDescriptions.directed} arrow>
                      <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                        <InfoOutlined fontSize="small" color="action" />
                        Ориентированный
                      </Box>
                    </Tooltip>
                  </StyledTableCell>
                  <StyledTableCell>{properties.directed ? 'Да' : 'Нет'}</StyledTableCell>
                </StyledTableRow>
                <StyledTableRow>
                  <StyledTableCell>
                    <Tooltip title={propertyDescriptions.nodeCount} arrow>
                      <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                        <InfoOutlined fontSize="small" color="action" />
                        Число вершин
                      </Box>
                    </Tooltip>
                  </StyledTableCell>
                  <StyledTableCell>{properties.nodeCount.toLocaleString()}</StyledTableCell>
                </StyledTableRow>
                <StyledTableRow>
                  <StyledTableCell>
                    <Tooltip title={propertyDescriptions.edgesCount} arrow>
                      <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                        <InfoOutlined fontSize="small" color="action" />
                        Число рёбер
                      </Box>
                    </Tooltip>
                  </StyledTableCell>
                  <StyledTableCell>{properties.edgesCount.toLocaleString()}</StyledTableCell>
                </StyledTableRow>
                <StyledTableRow>
                  <StyledTableCell>
                    <Tooltip title={propertyDescriptions.density} arrow>
                      <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                        <InfoOutlined fontSize="small" color="action" />
                        Плотность
                      </Box>
                    </Tooltip>
                  </StyledTableCell>
                  <StyledTableCell>{properties.density.toFixed(4)}</StyledTableCell>
                </StyledTableRow>
                <StyledTableRow>
                  <StyledTableCell>
                    <Tooltip title={propertyDescriptions.wccCount} arrow>
                      <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                        <InfoOutlined fontSize="small" color="action" />
                        Компоненты слабой связности
                      </Box>
                    </Tooltip>
                  </StyledTableCell>
                  <StyledTableCell>{properties.wccCount}</StyledTableCell>
                </StyledTableRow>
                <StyledTableRow>
                  <StyledTableCell>
                    <Tooltip title={propertyDescriptions.proportionWCC} arrow>
                      <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                        <InfoOutlined fontSize="small" color="action" />
                        Доля вершин в max WCC
                      </Box>
                    </Tooltip>
                  </StyledTableCell>
                  <StyledTableCell>{properties.proportionWCC.toFixed(2)}</StyledTableCell>
                </StyledTableRow>
                {properties.directed && (
                  <>
                    <StyledTableRow>
                      <StyledTableCell>
                        <Tooltip title={propertyDescriptions.sccCount} arrow>
                          <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                            <InfoOutlined fontSize="small" color="action" />
                            Компоненты сильной связности
                          </Box>
                        </Tooltip>
                      </StyledTableCell>
                      <StyledTableCell>{properties.sccCount ?? 'N/A'}</StyledTableCell>
                    </StyledTableRow>
                    <StyledTableRow>
                      <StyledTableCell>
                        <Tooltip title={propertyDescriptions.proportionSCC} arrow>
                          <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                            <InfoOutlined fontSize="small" color="action" />
                            Доля вершин в max SCC
                          </Box>
                        </Tooltip>
                      </StyledTableCell>
                      <StyledTableCell>{properties.proportionSCC?.toFixed(2) ?? 'N/A'}</StyledTableCell>
                    </StyledTableRow>
                  </>
                )}
              </TableBody>
            </Table>
            <Button variant="outlined" onClick={handleDownloadCsv} sx={{ mt: 2,}}>
              Скачать CSV
            </Button>
          </Box>
        )}

        {tabValue === 1 && (
          <Box sx={{ display: 'flex', gap: 3 }}>
            <Box sx={{ flex: 2 }}>
              <Typography variant="h6" gutterBottom>
                Визуализация тестового графа
              </Typography>
              <Box sx={{ mb: 2 }}>
                <Button variant="contained" color="primary" onClick={handleRunWCC} sx={{ mr: 2 }}>
                  Запустить WCC
                </Button>
                <Button variant="contained" color="secondary" onClick={handleRunSCC}>
                  Запустить SCC
                </Button>
              </Box>
              <CytoscapeComponent
                elements={graphElements}
                style={{ width: '100%', height: '500px', border: '1px solid #ccc' }}
                layout={{ name: 'cose' }}
                stylesheet={[
                  {
                    selector: 'node',
                    style: {
                      'background-color': '#666',
                      label: 'data(id)',
                      'text-valign': 'center',
                      color: 'white',
                      'text-outline-width': 2,
                      'text-outline-color': '#666',
                      width: '30px',
                      height: '30px',
                    },
                  },
                  {
                    selector: 'edge',
                    style: {
                      width: 3,
                      'line-color': '#ccc',
                      'target-arrow-color': '#ccc',
                      'target-arrow-shape': testGraph.directed ? 'triangle' : 'none',
                      'curve-style': 'bezier',
                    },
                  },
                ]}
                cy={cy => {
                  cyRef.current = cy;
                }}
              />
            </Box>
            <Card sx={{ flex: 1, maxHeight: '500px', overflowY: 'auto' }}>
              <CardContent>
                <Typography variant="h6" gutterBottom>
                  Логи алгоритма
                </Typography>
                {logs.length === 0 ? (
                  <Typography color="text.secondary">Запустите алгоритм, чтобы увидеть логи.</Typography>
                ) : (
                  logs.map((log, index) => (
                    <Typography key={index} variant="body2" sx={{ mb: 1 }}>
                      [{new Date(log.timestamp).toLocaleTimeString('ru-RU')}] {log.message}
                    </Typography>
                  ))
                )}
              </CardContent>
            </Card>
          </Box>
        )}
      </Box>
    </Fade>
  );
};

export default GeneralPropertiesComponent;