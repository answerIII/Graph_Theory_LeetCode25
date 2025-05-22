import React, { useEffect, useState } from 'react';
import { Table, TableBody, TableCell, TableHead, TableRow, Paper, Typography } from '@mui/material';

interface GeneralPropertiesComponentProps {
  graphId: string;
}

interface GeneralProperties {
  directed: number;
  num_vertices: number;
  num_edges: number;
  density: number;
  weak_comp_count: number;
  weak_largest_comp_size: number;
  weak_largest_comp_frac: number;
  strong_comp_count: number;
  strong_largest_comp_frac: number;
}

const GeneralPropertiesComponent: React.FC<GeneralPropertiesComponentProps> = ({ graphId }) => {
  const [properties, setProperties] = useState<GeneralProperties | null>(null);

  useEffect(() => {
    // GET /graphs/{graphId}/general_properties
    setProperties({
      directed: 0,
      num_vertices: 5242,
      num_edges: 14496,
      density: 0.001,
      weak_comp_count: 1,
      weak_largest_comp_size: 5242,
      weak_largest_comp_frac: 1.0,
      strong_comp_count: 5242,
      strong_largest_comp_frac: 0.2,
    });
  }, [graphId]);

  if (!properties) return <Typography>Загрузка...</Typography>;

  return (
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        Общие свойства графа
      </Typography>
      <Table>
        <TableHead>
          <TableRow>
            <TableCell>Характеристика</TableCell>
            <TableCell>Значение</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          <TableRow>
            <TableCell>Тип графа</TableCell>
            <TableCell>{properties.directed ? 'Ориентированный' : 'Неориентированный'}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Число вершин</TableCell>
            <TableCell>{properties.num_vertices}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Число рёбер</TableCell>
            <TableCell>{properties.num_edges}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Плотность</TableCell>
            <TableCell>{properties.density.toFixed(4)}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Число слабых компонент</TableCell>
            <TableCell>{properties.weak_comp_count}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Размер макс. слабой компоненты</TableCell>
            <TableCell>{properties.weak_largest_comp_size}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Доля вершин в макс. слабой компоненте</TableCell>
            <TableCell>{properties.weak_largest_comp_frac.toFixed(2)}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Число сильных компонент</TableCell>
            <TableCell>{properties.strong_comp_count}</TableCell>
          </TableRow>
          <TableRow>
            <TableCell>Доля вершин в макс. сильной компоненте</TableCell>
            <TableCell>{properties.strong_largest_comp_frac.toFixed(2)}</TableCell>
          </TableRow>
        </TableBody>
      </Table>
    </Paper>
  );
};

export default GeneralPropertiesComponent;