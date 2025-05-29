import { styled, TableCell, TableRow } from '@mui/material';

// Стилизованная ячейка таблицы
export const StyledTableCell = styled(TableCell)(({ theme }) => ({
  padding: theme.spacing(2, 3),
  '&:first-child': {
    fontWeight: theme.typography.fontWeightMedium,
    color: theme.palette.text.primary,
  },
  '&:last-child': {
    color: theme.palette.primary.dark,
  },
}));

// Стилизованная строка таблицы
export const StyledTableRow = styled(TableRow)(({ theme }) => ({
  '&:hover': {
    backgroundColor: theme.palette.action.hover,
  },
  transition: 'background-color 0.2s ease-in-out',
}));

// Описания характеристик для Tooltips
export const propertyDescriptions: Record<string, string> = {
  directed: 'Указывает, является ли граф ориентированным (с направленными рёбрами) или неориентированным.',
  nodeCount: 'Количество вершин (узлов) в графе.',
  edgesCount: 'Количество рёбер (связей) между вершинами.',
  density: 'Плотность графа — отношение числа рёбер к максимально возможному числу рёбер.',
  wccCount: 'Число компонент слабой связности — групп вершин, соединённых путями (игнорируя направление).',
  proportionWCC: 'Доля вершин, входящих в наибольшую компоненту слабой связности.',
  sccCount: 'Число компонент сильной связности — групп вершин, где каждая достижима из другой (с учётом направления).',
  proportionSCC: 'Доля вершин, входящих в наибольшую компоненту сильной связности.',
};