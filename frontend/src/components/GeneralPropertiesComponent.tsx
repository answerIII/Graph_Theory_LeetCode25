import React, { useEffect, useState } from 'react';
import { Table, TableBody, TableCell, TableRow, Typography, CircularProgress, Alert, Box } from '@mui/material';

interface GeneralPropertiesComponentProps {
  graphId: string;
}

interface GeneralProperties {
  directed: boolean;
  nodeCount: number;
  edgesCount: number;
  density: number;
  wccCount: number;
  proportionWCC: number;
  sccCount?: number;
  proportionSCC?: number;
}

const GeneralPropertiesComponent: React.FC<GeneralPropertiesComponentProps> = ({ graphId }) => {
  const [properties, setProperties] = useState<GeneralProperties | null>(null);
  // const [loading, setLoading] = useState(false);
  // const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    // GET /graphs/{graphId}/general_properties
    // const fetchProperties = async () => {
    //   try {
    //     setLoading(true);
    //     const response = await fetch(`/api/graphs/${graphId}/properties`);
    //     if (!response.ok) throw new Error('Ошибка сервера');
    //     const data: GeneralProperties = await response.json();
    //     setProperties(data);
    //   } catch (err) {
    //     setError((err as Error).message);
    //   } finally {
    //     setLoading(false);
    //   }
    // };
    // fetchProperties();
    setProperties({
      directed: true,
      nodeCount: 5242,
      edgesCount: 14496,
      density: 0.001,
      wccCount: 1,
      proportionWCC: 1,
      sccCount: 5242,
      proportionSCC: 0.2,
    });
  }, [graphId]);

  // if (loading) return <CircularProgress />;
  // if (error) return <Alert severity="error">{error}</Alert>;
  if (!properties) return <Typography>Загрузка...</Typography>;

  return (

    <Box>
      <Typography variant="h6" gutterBottom>
        Общие свойства графа
      </Typography>

      <Table>
      <TableBody>
        <TableRow><TableCell>Ориентированный</TableCell><TableCell>{properties.directed ? 'Да' : 'Нет'}</TableCell></TableRow>
        <TableRow><TableCell>Число вершин</TableCell><TableCell>{properties.nodeCount}</TableCell></TableRow>
        <TableRow><TableCell>Число рёбер</TableCell><TableCell>{properties.edgesCount}</TableCell></TableRow>
        <TableRow><TableCell>Плотность</TableCell><TableCell>{properties.density.toFixed(4)}</TableCell></TableRow>
        <TableRow><TableCell>Компоненты слабой связности</TableCell><TableCell>{properties.wccCount}</TableCell></TableRow>
        <TableRow><TableCell>Доля вершин в max WCC</TableCell><TableCell>{properties.proportionWCC.toFixed(4)}</TableCell></TableRow>
        {properties.directed && (
          <>
            <TableRow><TableCell>Компоненты сильной связности</TableCell><TableCell>{properties.sccCount}</TableCell></TableRow>
            <TableRow><TableCell>Доля вершин в max SCC</TableCell><TableCell>{properties.proportionSCC?.toFixed(4)}</TableCell></TableRow>
          </>
        )}
      </TableBody>
    </Table>
    </Box>
    
  );
};

export default GeneralPropertiesComponent;

// import { useEffect, useState } from 'react';
// import { graphApi } from '../api/graphApi';
// import type { GeneralPropertiesData } from '../types/graphTypes';
// import { CircularProgress, Alert, Table, TableBody, TableCell, TableRow } from '@mui/material';

// const GeneralPropertiesComponent: React.FC<{ datasetname: string }> = ({ datasetname }) => {
//   const [properties, setProperties] = useState<GeneralPropertiesData | null>(null);
//   const [loading, setLoading] = useState(false);
//   const [error, setError] = useState<string | null>(null);

//   useEffect(() => {
//     const fetchProperties = async () => {
//       try {
//         setLoading(true);
//         const data = await graphApi.getGeneralProperties('CA-GrQc');
//         setProperties(data);
//       } catch (err) {
//         setError((err as Error).message);
//       } finally {
//         setLoading(false);
//       }
//     };
//     fetchProperties();
//   }, [datasetname]);

//   if (loading) return <CircularProgress />;
//   if (error) return <Alert severity="error">{error}</Alert>;
//   if (!properties) return null;

//   return (
//     <Table>
//       <TableBody>
//         <TableRow><TableCell>Ориентированный</TableCell><TableCell>{properties.directed ? 'Да' : 'Нет'}</TableCell></TableRow>
//         <TableRow><TableCell>Число вершин</TableCell><TableCell>{properties.nodeCount}</TableCell></TableRow>
//         <TableRow><TableCell>Число рёбер</TableCell><TableCell>{properties.edgesCount}</TableCell></TableRow>
//         <TableRow><TableCell>Плотность</TableCell><TableCell>{properties.density.toFixed(4)}</TableCell></TableRow>
//         <TableRow><TableCell>Компоненты слабой связности</TableCell><TableCell>{properties.wccCount}</TableCell></TableRow>
//         <TableRow><TableCell>Доля вершин в max WCC</TableCell><TableCell>{properties.proportionWCC.toFixed(4)}</TableCell></TableRow>
//         {properties.directed && (
//           <>
//             <TableRow><TableCell>Компоненты сильной связности</TableCell><TableCell>{properties.sccCount}</TableCell></TableRow>
//             <TableRow><TableCell>Доля вершин в max SCC</TableCell><TableCell>{properties.proportionSCC?.toFixed(4)}</TableCell></TableRow>
//           </>
//         )}
//       </TableBody>
//     </Table>
//   );
// };

// export default GeneralPropertiesComponent;