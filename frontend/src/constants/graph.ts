import type { Graph } from '../types/interfaces';

// Тестовый граф для быстрого тестирования
export const testGraph: Graph = {
  directed: false,
  numNodes: 10,
  edges: [
    [0, 1],
    [0, 2],
    [1, 2],
    [1, 3],
    [2, 3],
    [3, 4],
    [4, 5],
    [5, 6],
    [6, 7],
    [7, 8],
    [8, 9],
    [9, 0],
    [2, 4],
    [4, 6],
    [6, 8],
  ],
};

// Список доступных датасетов
export const datasets = [
    'CA-AstroPh',
    'ca-coauthors-dblp',
    'CA-GrQc',
    'Email-EuAll',
    'musae_git_edges',
];