// import { useEffect, useState, useRef } from 'react';
// import { graphApi } from '../../api/graphApi';
// import type { GeneralPropertiesData } from '../../types/graphTypes';
// import {
//   CircularProgress,
//   Alert,
//   Table,
//   TableBody,
//   TableCell,
//   TableHead,
//   TableRow,
//   Box,
//   Typography,
//   Tooltip,
//   Fade,
//   styled,
//   Tabs,
//   Tab,
//   Button,
//   Card,
//   CardContent,
// } from '@mui/material';
// import { InfoOutlined } from '@mui/icons-material';
// import CytoscapeComponent from 'react-cytoscapejs';
// import type { Core as Cy } from 'cytoscape';

// // Типы для графа
// interface Graph {
//   directed: boolean;
//   vertexCount: number;
//   edgesCount: number;
//   adjList: Map<string, string[]>;
//   edges: Array<{ from: string; to: string }>;
// }

// interface StatDFS {
//   vertex: string;
//   timeIn: number;
//   timeOut: number;
// }

// interface LogEntry {
//   message: string;
//   timestamp: number;
// }

// // Описания характеристик
// const propertyDescriptions: Record<string, string> = {
//   directed: 'Указывает, является ли граф ориентированным (с направленными рёбрами) или неориентированным.',
//   nodeCount: 'Количество вершин (узлов) в графе.',
//   edgesCount: 'Количество рёбер (связей) между вершинами.',
//   density: 'Плотность графа — отношение числа рёбер к максимально возможному числу рёбер.',
//   wccCount: 'Число компонент слабой связности — групп вершин, соединённых путями (игнорируя направление).',
//   proportionWCC: 'Доля вершин, входящих в наибольшую компоненту слабой связности.',
//   sccCount: 'Число компонент сильной связности — групп вершин, где каждая достижима из другой (с учётом направления).',
//   proportionSCC: 'Доля вершин, входящих в наибольшую компоненту сильной связности.',
// };

// // Стили
// const StyledTableCell = styled(TableCell)(({ theme }) => ({
//   padding: theme.spacing(2, 3),
//   '&:first-child': {
//     fontWeight: theme.typography.fontWeightMedium,
//     color: theme.palette.text.primary,
//   },
//   '&:last-child': {
//     color: theme.palette.primary.dark,
//   },
// }));

// const StyledTableRow = styled(TableRow)(({ theme }) => ({
//   '&:hover': {
//     backgroundColor: theme.palette.action.hover,
//   },
//   transition: 'background-color 0.2s ease-in-out',
// }));

// // Тестовый граф
// const testGraph: Graph = {
//   directed: true,
//   vertexCount: 10,
//   edgesCount: 15,
//   adjList: new Map([
//     ['0', ['1', '3']],
//     ['1', ['2', '4']],
//     ['2', ['0', '5']],
//     ['3', ['4', '9']],
//     ['4', ['5', '6']],
//     ['5', ['3', '2']],
//     ['6', ['7']],
//     ['7', ['8']],
//     ['8', ['9']],
//     ['9', ['8']],
//   ]),
//   edges: [
//     { from: '0', to: '1' },
//     { from: '1', to: '2' },
//     { from: '2', to: '0' },
//     { from: '0', to: '3' },
//     { from: '3', to: '4' },
//     { from: '4', to: '5' },
//     { from: '5', to: '3' },
//     { from: '1', to: '4' },
//     { from: '2', to: '5' },
//     { from: '3', to: '9' },
//     { from: '4', to: '6' },
//     { from: '6', to: '7' },
//     { from: '7', to: '8' },
//     { from: '8', to: '9' },
//     { from: '9', to: '8' },
//   ],
// };

// // Алгоритмы
// const findMaxWCC = async (
//   graph: Graph,
//   cy: Cy,
//   addLog: (message: string) => void
// ): Promise<{ maxWccSize: number; countWCC: number }> => {
//   const visited = new Set<string>();
//   let maxWccSize = 0;
//   let countWCC = 0;
//   const componentColors = [
//     '#4caf50',
//     '#2196f0',
//     '#ff5722',
//     '#9c27b0',
//     '#ffeb3b',
//     '#00bcd4',
//     '#e91e63',
//     '#8bc34a',
//   ];

//   for (const vertex of graph.adjList.keys()) {
//     if (visited.has(vertex)) continue;

//     countWCC++;
//     const component = new Set<string>();
//     const queue: string[] = [vertex];
//     component.add(vertex);
//     visited.add(vertex);

//     cy.getElementById(vertex).style({ 'background-color': componentColors[countWCC - 1] });
//     addLog(`Начинаем BFS с вершины ${vertex} для WCC #${countWCC}`);
//     await new Promise(resolve => setTimeout(resolve, 500));

//     while (queue.length > 0) {
//       const current = queue.shift()!;
//       addLog(`Посещаем вершину ${current}`);

//       for (const neighbor of graph.adjList.get(current) || []) {
//         if (!visited.has(neighbor)) {
//           queue.push(neighbor);
//           component.add(neighbor);
//           visited.add(neighbor);

//           cy.getElementById(neighbor).style({ 'background-color': componentColors[countWCC - 1] });
//           cy.getElementById(`${current}_${neighbor}`).style({ 'line-color': componentColors[countWCC - 1] });
//           addLog(`Обнаружено ребро ${current} → ${neighbor}`);
//           await new Promise(resolve => setTimeout(resolve, 500));
//         }
//       }
//     }

//     maxWccSize = Math.max(maxWccSize, component.size);
//     addLog(`Найдена WCC #${countWCC} с ${component.size} вершинами`);
//   }

//   addLog(`Итог: найдено ${countWCC} WCC, наибольшая содержит ${maxWccSize} вершин`);
//   return { maxWccSize, countWCC };
// };

// const findMaxSCC = async (
//   graph: Graph,
//   cy: Cy,
//   addLog: (message: string) => void
// ): Promise<{ maxSccSize: number; countScc: number }> => {
//   // Инвертируем граф
//   const invGraph: Graph = {
//     directed: graph.directed,
//     vertexCount: graph.vertexCount,
//     edgesCount: graph.edgesCount,
//     adjList: new Map(),
//     edges: graph.edges.map(edge => ({ from: edge.to, to: edge.from })),
//   };
//   for (const vertex of graph.adjList.keys()) {
//     invGraph.adjList.set(vertex, []);
//   }
//   for (const edge of invGraph.edges) {
//     invGraph.adjList.get(edge.from)!.push(edge.to);
//   }
//   addLog('Инвертирован граф для первого DFS');

//   // Первый DFS
//   const dfsResult = await timeDFS(invGraph, addLog, cy, 'blue');
//   const dfsList = Object.values(dfsResult).sort((a, b) => b.timeOut - a.timeOut);
//   addLog('Завершён первый DFS, вершины отсортированы по времени выхода');

//   // Второй DFS
//   const visited = new Set<string>();
//   let maxSccSize = 0;
//   let countScc = 0;
//   const componentColors = [
//     '#4caf50',
//     '#2196f0',
//     '#ff5722',
//     '#9c27b0',
//     '#ffeb3b',
//     '#00bcd4',
//     '#e91e63',
//     '#8bc34a',
//   ];

//   for (const { vertex } of dfsList) {
//     if (visited.has(vertex)) continue;

//     countScc++;
//     const component = new Set<string>();
//     const stack: string[] = [vertex];
//     component.add(vertex);
//     visited.add(vertex);

//     cy.getElementById(vertex).style({ 'background-color': componentColors[countScc - 1] });
//     addLog(`Начинаем DFS для SCC #${countScc} с вершины ${vertex}`);
//     await new Promise(resolve => setTimeout(resolve, 500));

//     while (stack.length > 0) {
//       const current = stack.pop()!;
//       addLog(`Посещаем вершину ${current}`);

//       for (const neighbor of graph.adjList.get(current) || []) {
//         if (!visited.has(neighbor)) {
//           stack.push(neighbor);
//           component.add(neighbor);
//           visited.add(neighbor);

//           cy.getElementById(neighbor).style({ 'background-color': componentColors[countScc - 1] });
//           cy.getElementById(`${current}_${neighbor}`).style({ 'line-color': componentColors[countScc - 1] });
//           addLog(`Обнаружено ребро ${current} → ${neighbor}`);
//           await new Promise(resolve => setTimeout(resolve, 500));
//         }
//       }
//     }

//     maxSccSize = Math.max(maxSccSize, component.size);
//     addLog(`Найдена SCC #${countScc} с ${component.size} вершинами`);
//   }

//   addLog(`Итог: найдено ${countScc} SCC, наибольшая содержит ${maxSccSize} вершин`);
//   return { maxSccSize, countScc };
// };

// const timeDFS = async (
//   graph: Graph,
//   addLog: (message: string) => void,
//   cy: Cy,
//   color: string
// ): Promise<Record<string, StatDFS>> => {
//   const visited: Record<string, StatDFS> = {};
//   let time = 1;

//   for (const vertex of graph.adjList.keys()) {
//     if (vertex in visited) continue;

//     const stack: string[] = [vertex];
//     visited[vertex] = { vertex, timeIn: ++time, timeOut: 0 };
//     addLog(`DFS: Входим в вершину ${vertex}`);
//     cy.getElementById(vertex).style({ 'background-color': color });
//     await new Promise(resolve => setTimeout(resolve, 500));

//     while (stack.length > 0) {
//       const current = stack[stack.length - 1];
//       let added = false;

//       for (const neighbor of graph.adjList.get(current) || []) {
//         if (!(neighbor in visited)) {
//           stack.push(neighbor);
//           visited[neighbor] = { vertex: neighbor, timeIn: ++time, timeOut: 0 };
//           addLog(`DFS: Входим в вершину ${neighbor}`);
//           cy.getElementById(neighbor).style({ 'background-color': color });
//           cy.getElementById(`${current}_${neighbor}`).style({ 'line-color': color });
//           await new Promise(resolve => setTimeout(resolve, 500));
//           added = true;
//           break;
//         }
//       }

//       if (!added) {
//         visited[current].timeOut = ++time;
//         addLog(`DFS: Выходим из вершины ${current}`);
//         stack.pop();
//       }
//     }
//   }

//   return visited;
// };

// const GeneralPropertiesComponent: React.FC<{ datasetname: string }> = ({ datasetname }) => {
//   const [properties, setProperties] = useState<GeneralPropertiesData | null>(null);
//   const [loading, setLoading] = useState(false);
//   const [error, setError] = useState<string | null>(null);
//   const [tabValue, setTabValue] = useState(0);
//   const [logs, setLogs] = useState<LogEntry[]>([]);
//   const cyRef = useRef<Cy | null>(null);

//   useEffect(() => {
//     const fetchData = async () => {
//       try {
//         setLoading(true);
//         const data = await graphApi.getGeneralProperties(datasetname);
//         setProperties(data);
//       } catch (err) {
//         setError((err as Error).message);
//       } finally {
//         setLoading(false);
//       }
//     };
//     fetchData();
//   }, [datasetname]);

//   const addLog = (message: string) => {
//     setLogs(prev => [...prev, { message, timestamp: Date.now() }]);
//   };

//   const handleRunWCC = async () => {
//     if (!cyRef.current) return;
//     setLogs([]);
//     const { maxWccSize, countWCC } = await findMaxWCC(testGraph, cyRef.current, addLog);
//     addLog(`Итог: найдено ${countWCC} WCC, наибольшая содержит ${maxWccSize} вершин`);
//   };

//   const handleRunSCC = async () => {
//     if (!cyRef.current) return;
//     setLogs([]);
//     const { maxSccSize, countScc } = await findMaxSCC(testGraph, cyRef.current, addLog);
//     addLog(`Итог: найдено ${countScc} SCC, наибольшая содержит ${maxSccSize} вершин`);
//   };

//   const graphElements = [
//     ...Array.from(testGraph.adjList.keys()).map(id => ({ data: { id } })),
//     ...testGraph.edges.map(edge => ({
//       data: { id: `${edge.from}_${edge.to}`, source: edge.from, target: edge.to },
//     })),
//   ];

//   if (loading) return <CircularProgress />;
//   if (error) return <Alert severity="error">{error}</Alert>;
//   if (!properties) return <Typography>Загрузка данных...</Typography>;

//   return (
//     <Fade in timeout={600}>
//       <Box sx={{ p: 3, bgcolor: 'background.paper', borderRadius: 2, boxShadow: 3, maxWidth: '1200px', mx: 'auto' }}>
//         <Typography variant="h5" gutterBottom color="primary.main">
//           Общие свойства графа: {datasetname}
//         </Typography>
//         <Tabs value={tabValue} onChange={(_e, newValue) => setTabValue(newValue)} sx={{ mb: 3 }}>
//           <Tab label="Свойства" />
//           <Tab label="Алгоритмы" />
//         </Tabs>

//         {tabValue === 0 && (
//           <Box>
//             <Typography variant="body2" color="text.secondary" mb={3}>
//               Основные метрики графа, включая топологические характеристики и структуру связности.
//             </Typography>
//             <Table sx={{ border: '1px solid', borderColor: 'grey.200' }}>
//               <TableHead>
//                 <TableRow>
//                   <StyledTableCell>Характеристика</StyledTableCell>
//                   <StyledTableCell>Значение</StyledTableCell>
//                 </TableRow>
//               </TableHead>
//               <TableBody>
//                 <StyledTableRow>
//                   <StyledTableCell>
//                     <Tooltip title={propertyDescriptions.directed} arrow>
//                       <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
//                         <InfoOutlined fontSize="small" color="action" />
//                         Ориентированный
//                       </Box>
//                     </Tooltip>
//                   </StyledTableCell>
//                   <StyledTableCell>{properties.directed ? 'Да' : 'Нет'}</StyledTableCell>
//                 </StyledTableRow>
//                 <StyledTableRow>
//                   <StyledTableCell>
//                     <Tooltip title={propertyDescriptions.nodeCount} arrow>
//                       <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
//                         <InfoOutlined fontSize="small" color="action" />
//                         Число вершин
//                       </Box>
//                     </Tooltip>
//                   </StyledTableCell>
//                   <StyledTableCell>{properties.nodeCount.toLocaleString()}</StyledTableCell>
//                 </StyledTableRow>
//                 <StyledTableRow>
//                   <StyledTableCell>
//                     <Tooltip title={propertyDescriptions.edgesCount} arrow>
//                       <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
//                         <InfoOutlined fontSize="small" color="action" />
//                         Число рёбер
//                       </Box>
//                     </Tooltip>
//                   </StyledTableCell>
//                   <StyledTableCell>{properties.edgesCount.toLocaleString()}</StyledTableCell>
//                 </StyledTableRow>
//                 <StyledTableRow>
//                   <StyledTableCell>
//                     <Tooltip title={propertyDescriptions.density} arrow>
//                       <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
//                         <InfoOutlined fontSize="small" color="action" />
//                         Плотность
//                       </Box>
//                     </Tooltip>
//                   </StyledTableCell>
//                   <StyledTableCell>{properties.density.toFixed(4)}</StyledTableCell>
//                 </StyledTableRow>
//                 <StyledTableRow>
//                   <StyledTableCell>
//                     <Tooltip title={propertyDescriptions.wccCount} arrow>
//                       <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
//                         <InfoOutlined fontSize="small" color="action" />
//                         Компоненты слабой связности
//                       </Box>
//                     </Tooltip>
//                   </StyledTableCell>
//                   <StyledTableCell>{properties.wccCount}</StyledTableCell>
//                 </StyledTableRow>
//                 <StyledTableRow>
//                   <StyledTableCell>
//                     <Tooltip title={propertyDescriptions.proportionWCC} arrow>
//                       <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
//                         <InfoOutlined fontSize="small" color="action" />
//                         Доля вершин в max WCC
//                       </Box>
//                     </Tooltip>
//                   </StyledTableCell>
//                   <StyledTableCell>{properties.proportionWCC.toFixed(2)}</StyledTableCell>
//                 </StyledTableRow>
//                 {properties.directed && (
//                   <>
//                     <StyledTableRow>
//                       <StyledTableCell>
//                         <Tooltip title={propertyDescriptions.sccCount} arrow>
//                           <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
//                             <InfoOutlined fontSize="small" color="action" />
//                             Компоненты сильной связности
//                           </Box>
//                         </Tooltip>
//                       </StyledTableCell>
//                       <StyledTableCell>{properties.sccCount ?? 'N/A'}</StyledTableCell>
//                     </StyledTableRow>
//                     <StyledTableRow>
//                       <StyledTableCell>
//                         <Tooltip title={propertyDescriptions.proportionSCC} arrow>
//                           <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
//                             <InfoOutlined fontSize="small" color="action" />
//                             Доля вершин в max SCC
//                           </Box>
//                         </Tooltip>
//                       </StyledTableCell>
//                       <StyledTableCell>{properties.proportionSCC?.toFixed(2) ?? 'N/A'}</StyledTableCell>
//                     </StyledTableRow>
//                   </>
//                 )}
//               </TableBody>
//             </Table>
//           </Box>
//         )}

//         {tabValue === 1 && (
//           <Box sx={{ display: 'flex', gap: 3 }}>
//             <Box sx={{ flex: 2 }}>
//               <Typography variant="h6" gutterBottom>
//                 Визуализация тестового графа
//               </Typography>
//               <Box sx={{ mb: 2 }}>
//                 <Button variant="contained" color="primary" onClick={handleRunWCC} sx={{ mr: 2 }}>
//                   Запустить WCC
//                 </Button>
//                 <Button variant="contained" color="secondary" onClick={handleRunSCC}>
//                   Запустить SCC
//                 </Button>
//               </Box>
//               <CytoscapeComponent
//                 elements={graphElements}
//                 style={{ width: '100%', height: '500px', border: '1px solid #ccc' }}
//                 layout={{ name: 'cose' }}
//                 stylesheet={[
//                   {
//                     selector: 'node',
//                     style: {
//                       'background-color': '#666',
//                       label: 'data(id)',
//                       'text-valign': 'center',
//                       color: 'white',
//                       'text-outline-width': 2,
//                       'text-outline-color': '#666',
//                       width: '30px',
//                       height: '30px',
//                     },
//                   },
//                   {
//                     selector: 'edge',
//                     style: {
//                       width: 3,
//                       'line-color': '#ccc',
//                       'target-arrow-color': '#ccc',
//                       'target-arrow-shape': testGraph.directed ? 'triangle' : 'none',
//                       'curve-style': 'bezier',
//                     },
//                   },
//                 ]}
//                 cy={cy => {
//                   cyRef.current = cy;
//                 }}
//               />
//             </Box>
//             <Card sx={{ flex: 1, maxHeight: '500px', overflowY: 'auto' }}>
//               <CardContent>
//                 <Typography variant="h6" gutterBottom>
//                   Логи алгоритма
//                 </Typography>
//                 {logs.length === 0 ? (
//                   <Typography color="text.secondary">Запустите алгоритм, чтобы увидеть логи.</Typography>
//                 ) : (
//                   logs.map((log, index) => (
//                     <Typography key={index} variant="body2" sx={{ mb: 1 }}>
//                       [{new Date(log.timestamp).toLocaleTimeString('ru-RU')}] {log.message}
//                     </Typography>
//                   ))
//                 )}
//               </CardContent>
//             </Card>
//           </Box>
//         )}
//       </Box>
//     </Fade>
//   );
// };

// export default GeneralPropertiesComponent;

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
import { graphApi } from '../../api/graphApi';
import type { GeneralPropertiesData, LogEntry } from './types';
import { findMaxWCC, findMaxSCC } from './graphAlgorithms';
import { testGraph } from './testGraph';
import { StyledTableCell, StyledTableRow, propertyDescriptions } from './styles';
import { InfoOutlined } from '@mui/icons-material';

// Основной компонент для отображения свойств графа и визуализации алгоритмов
const GeneralPropertiesComponent: React.FC<{ datasetname: string }> = ({ datasetname }) => {
  const [properties, setProperties] = useState<GeneralPropertiesData | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [tabValue, setTabValue] = useState(0);
  const [logs, setLogs] = useState<LogEntry[]>([]);
  const cyRef = useRef<Cy | null>(null);

  // Загрузка данных с бэкенда
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

  // Добавление лога
  const addLog = (message: string) => {
    setLogs(prev => [...prev, { message, timestamp: Date.now() }]);
  };

  // Запуск алгоритма WCC
  const handleRunWCC = async () => {
    if (!cyRef.current) return;
    setLogs([]);
    const { maxWccSize, countWCC } = await findMaxWCC(testGraph, cyRef.current, addLog);
    addLog(`Итог: найдено ${countWCC} WCC, наибольшая содержит ${maxWccSize} вершин`);
  };

  // Запуск алгоритма SCC
  const handleRunSCC = async () => {
    if (!cyRef.current) return;
    setLogs([]);
    const { maxSccSize, countScc } = await findMaxSCC(testGraph, cyRef.current, addLog);
    addLog(`Итог: найдено ${countScc} SCC, наибольшая содержит ${maxSccSize} вершин`);
  };

  // Элементы графа для Cytoscape
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
          <Box>
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