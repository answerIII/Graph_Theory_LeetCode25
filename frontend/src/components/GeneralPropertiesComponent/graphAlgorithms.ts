import type { Core as Cy } from 'cytoscape';
import type { Graph, StatDFS } from './types';

// Поиск компонент слабой связности (WCC)
export const findMaxWCC = async (
  graph: Graph,
  cy: Cy,
  addLog: (message: string) => void
): Promise<{ maxWccSize: number; countWCC: number }> => {
  const visited = new Set<string>();
  let maxWccSize = 0;
  let countWCC = 0;
  const componentColors = [
    '#4caf50',
    '#2196f0',
    '#ff5722',
    '#9c27b0',
    '#ffeb3b',
    '#00bcd4',
    '#e91e63',
    '#8bc34a',
  ];

  for (const vertex of graph.adjList.keys()) {
    if (visited.has(vertex)) continue;

    countWCC++;
    const component = new Set<string>();
    const queue: string[] = [vertex];
    component.add(vertex);
    visited.add(vertex);

    cy.getElementById(vertex).style({ 'background-color': componentColors[countWCC - 1] });
    addLog(`Начинаем BFS с вершины ${vertex} для WCC #${countWCC}`);
    await new Promise(resolve => setTimeout(resolve, 500));

    while (queue.length > 0) {
      const current = queue.shift()!;
      addLog(`Посещаем вершину ${current}`);

      for (const neighbor of graph.adjList.get(current) || []) {
        if (!visited.has(neighbor)) {
          queue.push(neighbor);
          component.add(neighbor);
          visited.add(neighbor);

          cy.getElementById(neighbor).style({ 'background-color': componentColors[countWCC - 1] });
          cy.getElementById(`${current}_${neighbor}`).style({ 'line-color': componentColors[countWCC - 1] });
          addLog(`Обнаружено ребро ${current} → ${neighbor}`);
          await new Promise(resolve => setTimeout(resolve, 500));
        }
      }
    }

    maxWccSize = Math.max(maxWccSize, component.size);
    addLog(`Найдена WCC #${countWCC} с ${component.size} вершинами`);
  }

  addLog(`Итог: найдено ${countWCC} WCC, наибольшая содержит ${maxWccSize} вершин`);
  return { maxWccSize, countWCC };
};

// Поиск компонент сильной связности (SCC)
export const findMaxSCC = async (
  graph: Graph,
  cy: Cy,
  addLog: (message: string) => void
): Promise<{ maxSccSize: number; countScc: number }> => {
  // Инвертируем граф
  const invGraph: Graph = {
    directed: graph.directed,
    vertexCount: graph.vertexCount,
    edgesCount: graph.edgesCount,
    adjList: new Map(),
    edges: graph.edges.map(edge => ({ from: edge.to, to: edge.from })),
  };
  for (const vertex of graph.adjList.keys()) {
    invGraph.adjList.set(vertex, []);
  }
  for (const edge of invGraph.edges) {
    invGraph.adjList.get(edge.from)!.push(edge.to);
  }
  addLog('Инвертирован граф для первого DFS');

  // Первый DFS
  const dfsResult = await timeDFS(invGraph, addLog, cy, 'blue');
  const dfsList = Object.values(dfsResult).sort((a, b) => b.timeOut - a.timeOut);
  addLog('Завершён первый DFS, вершины отсортированы по времени выхода');

  // Второй DFS
  const visited = new Set<string>();
  let maxSccSize = 0;
  let countScc = 0;
  const componentColors = [
    '#4caf50',
    '#2196f0',
    '#ff5722',
    '#9c27b0',
    '#ffeb3b',
    '#00bcd4',
    '#e91e63',
    '#8bc34a',
  ];

  for (const { vertex } of dfsList) {
    if (visited.has(vertex)) continue;

    countScc++;
    const component = new Set<string>();
    const stack: string[] = [vertex];
    component.add(vertex);
    visited.add(vertex);

    cy.getElementById(vertex).style({ 'background-color': componentColors[countScc - 1] });
    addLog(`Начинаем DFS для SCC #${countScc} с вершины ${vertex}`);
    await new Promise(resolve => setTimeout(resolve, 500));

    while (stack.length > 0) {
      const current = stack.pop()!;
      addLog(`Посещаем вершину ${current}`);

      for (const neighbor of graph.adjList.get(current) || []) {
        if (!visited.has(neighbor)) {
          stack.push(neighbor);
          component.add(neighbor);
          visited.add(neighbor);

          cy.getElementById(neighbor).style({ 'background-color': componentColors[countScc - 1] });
          cy.getElementById(`${current}_${neighbor}`).style({ 'line-color': componentColors[countScc - 1] });
          addLog(`Обнаружено ребро ${current} → ${neighbor}`);
          await new Promise(resolve => setTimeout(resolve, 500));
        }
      }
    }

    maxSccSize = Math.max(maxSccSize, component.size);
    addLog(`Найдена SCC #${countScc} с ${component.size} вершинами`);
  }

  addLog(`Итог: найдено ${countScc} SCC, наибольшая содержит ${maxSccSize} вершин`);
  return { maxSccSize, countScc };
};

// Вспомогательный DFS для времени входа/выхода
const timeDFS = async (
  graph: Graph,
  addLog: (message: string) => void,
  cy: Cy,
  color: string
): Promise<Record<string, StatDFS>> => {
  const visited: Record<string, StatDFS> = {};
  let time = 1;

  for (const vertex of graph.adjList.keys()) {
    if (vertex in visited) continue;

    const stack: string[] = [vertex];
    visited[vertex] = { vertex, timeIn: ++time, timeOut: 0 };
    addLog(`DFS: Входим в вершину ${vertex}`);
    cy.getElementById(vertex).style({ 'background-color': color });
    await new Promise(resolve => setTimeout(resolve, 500));

    while (stack.length > 0) {
      const current = stack[stack.length - 1];
      let added = false;

      for (const neighbor of graph.adjList.get(current) || []) {
        if (!(neighbor in visited)) {
          stack.push(neighbor);
          visited[neighbor] = { vertex: neighbor, timeIn: ++time, timeOut: 0 };
          addLog(`DFS: Входим в вершину ${neighbor}`);
          cy.getElementById(neighbor).style({ 'background-color': color });
          cy.getElementById(`${current}_${neighbor}`).style({ 'line-color': color });
          await new Promise(resolve => setTimeout(resolve, 500));
          added = true;
          break;
        }
      }

      if (!added) {
        visited[current].timeOut = ++time;
        addLog(`DFS: Выходим из вершины ${current}`);
        stack.pop();
      }
    }
  }

  return visited;
};