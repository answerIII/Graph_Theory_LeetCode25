import React, { useState, Component } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import {
  Box,
  Typography,
  Container,
  Tabs,
  Tab,
  Button,
  Alert,
  styled,
} from '@mui/material';
import { Accordion, AccordionSummary, AccordionDetails } from '@mui/material';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';

import GeneralPropertiesComponent from '../components/GeneralPropertiesComponent/GeneralPropertiesComponent';
import DistanceEstimationComponent from '../components/DistanceEstimationComponent';
import ClusteringComponent from '../components/ClusteringComponent';
import DegreeDistributionComponent from '../components/DegreeDistributionComponent';
import RobustnessComponent from '../components/RobustnessComponent';
import GraphVisualizationComponent from '../components/GraphVisualizationComponent';
import DistanceAnalysisComponent from '../components/DistanceAnalysisComponent';

// ErrorBoundary для изоляции ошибок
class ErrorBoundary extends Component<{ children: React.ReactNode }, { hasError: boolean }> {
  state = { hasError: false };

  static getDerivedStateFromError() {
    return { hasError: true };
  }

  render() {
    if (this.state.hasError) {
      return <Alert severity="error">Ошибка в компоненте</Alert>;
    }
    return this.props.children;
  }
}

// Стилизованный Tabs для улучшенной прокрутки
const StyledTabs = styled(Tabs)(({ theme }) => ({
  '& .MuiTabs-flexContainer': {
    gap: theme.spacing(1),
  },
  '& .MuiTabs-scroller': {
    overflowX: 'auto !important', // Гарантируем прокрутку
  },
  '& .MuiTab-root': {
    minWidth: 'auto',
    padding: theme.spacing(1, 2),
    fontSize: '0.9rem',
    textTransform: 'none',
    [theme.breakpoints.down('sm')]: {
      fontSize: '0.8rem',
      padding: theme.spacing(1),
    },
  },
}));

const AnalysisPage: React.FC = () => {
  const { datasetname } = useParams<{ datasetname: string }>();
  const [tabValue, setTabValue] = useState(0);
  const navigate = useNavigate();

  const handleTabChange = (event: React.SyntheticEvent, newValue: number) => {
    setTabValue(newValue);
  };

  const handleBack = () => {
    navigate('/');
  };

  // Список вкладок с метками и компонентами
  const tabs = [
    { label: 'Визуализация', component: <GraphVisualizationComponent datasetname={datasetname} /> },
    { label: 'Общие свойства', component: <GeneralPropertiesComponent datasetname={datasetname!} /> },
    { label: 'Оценка расстояний', component: <DistanceEstimationComponent datasetname={datasetname} /> },
    { label: 'Кластеризация', component: <ClusteringComponent datasetname={datasetname} /> },
    { label: 'Распределение степеней', component: <DegreeDistributionComponent datasetname={datasetname} /> },
    { label: 'Устойчивость', component: <RobustnessComponent datasetname={datasetname} /> },
    { label: 'Вычисление расстояний', component: <DistanceAnalysisComponent datasetname={datasetname} /> },
  ];

  if (!datasetname) {
    return (
      <Container maxWidth="lg" sx={{ mt: 4 }}>
        <Alert severity="error">Dataset name is not provided</Alert>
        <Button variant="outlined" onClick={handleBack} sx={{ mt: 2 }}>
          Назад
        </Button>
      </Container>
    );
  }

//   return (
//     <Container maxWidth="lg" sx={{ mt: 4 }}>
//       <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 2 }}>
//         <Typography variant="h4">Анализ графа: {datasetname}</Typography>
//         <Button variant="outlined" onClick={handleBack}>
//           Назад
//         </Button>
//       </Box>
//       <StyledTabs
//         value={tabValue}
//         onChange={handleTabChange}
//         variant="scrollable"
//         scrollButtons="auto"
//         aria-label="Анализ графа вкладки"
//         sx={{ mb: 2 }}
//       >
//         {tabs.map((tab, index) => (
//           <Tab key={index} label={tab.label} />
//         ))}
//       </StyledTabs>
//       <Box sx={{ p: 3 }}>
//         <ErrorBoundary>
//           {tabs[tabValue].component}
//         </ErrorBoundary>
//       </Box>
//     </Container>
//   );
// };

// export default AnalysisPage;

const [showAll, setShowAll] = useState(false);

  return (
    <Container maxWidth="lg" sx={{ mt: 4 }}>
      <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 2 }}>
         <Typography variant="h4">Анализ графа: {datasetname}</Typography>
         <Button variant="outlined" onClick={handleBack}>
           Назад
         </Button>
       </Box>
      <Box sx={{ mb: 2, textAlign: 'right' }}>
        <Button variant="outlined" onClick={() => setShowAll(!showAll)}>
          {showAll ? 'Показать вкладки' : 'Показать все'}
        </Button>
      </Box>
      {showAll ? (
        <Box sx={{ p: 3 }}>
          {tabs.map((tab, index) => (
            <Accordion key={index}>
              <AccordionSummary expandIcon={<ExpandMoreIcon />}>
                <Typography>{tab.label}</Typography>
              </AccordionSummary>
              <AccordionDetails>
                <ErrorBoundary>
                  {tab.component}
                </ErrorBoundary>
              </AccordionDetails>
            </Accordion>
          ))}
        </Box>
      ) : (
        <>
          <StyledTabs
            value={tabValue}
            onChange={handleTabChange}
            variant="scrollable"
            scrollButtons="auto"
            aria-label="Анализ графа вкладки"
            sx={{ mb: 2 }}
          >
            {tabs.map((tab, index) => (
              <Tab key={index} label={tab.label} />
            ))}
          </StyledTabs>
          <Box sx={{ p: 3 }}>
            <ErrorBoundary>
              {tabs[tabValue].component}
            </ErrorBoundary>
          </Box>
        </>
      )}
    </Container>
  );
};

export default AnalysisPage;