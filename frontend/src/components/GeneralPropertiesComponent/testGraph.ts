import type { Graph } from './types';

// Тестовый граф с 10 вершинами и 15 рёбрами
export const testGraph: Graph = {
  directed: true,
  vertexCount: 10,
  edgesCount: 15,
  adjList: new Map([
    ['0', ['1', '3']],
    ['1', ['2', '4']],
    ['2', ['0', '5']],
    ['3', ['4', '9']],
    ['4', ['5', '6']],
    ['5', ['3', '2']],
    ['6', ['7']],
    ['7', ['8']],
    ['8', ['9']],
    ['9', ['8']],
  ]),
  edges: [
    { from: '0', to: '1' },
    { from: '1', to: '2' },
    { from: '2', to: '0' },
    { from: '0', to: '3' },
    { from: '3', to: '4' },
    { from: '4', to: '5' },
    { from: '5', to: '3' },
    { from: '1', to: '4' },
    { from: '2', to: '5' },
    { from: '3', to: '9' },
    { from: '4', to: '6' },
    { from: '6', to: '7' },
    { from: '7', to: '8' },
    { from: '8', to: '9' },
    { from: '9', to: '8' },
  ],
};