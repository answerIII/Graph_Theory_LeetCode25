import React, { useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { Box, Typography, Container, Tabs, Tab, Button } from '@mui/material';
import GeneralPropertiesComponent from '../components/GeneralPropertiesComponent/GeneralPropertiesComponent';
import DistanceEstimationComponent from '../components/DistanceEstimationComponent';
import ClusteringComponent from '../components/ClusteringComponent';
import DegreeDistributionComponent from '../components/DegreeDistributionComponent';
import RobustnessComponent from '../components/RobustnessComponent';
import AlgorithmsRuntimeComponent from '../components/AlgorithmsRuntimeComponent';
import GraphVisualizationComponent from '../components/GraphVisualizationComponent';

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

  return (
    <Container maxWidth="lg" sx={{ mt: 4 }}>
      <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 2 }}>
        <Typography variant="h4">
          Анализ графа: {datasetname}
        </Typography>
        <Button variant="outlined" onClick={handleBack}>
          Назад
        </Button>
      </Box>
      <Tabs value={tabValue} onChange={handleTabChange} centered>
        <Tab label="Визуализация" />
        <Tab label="Общие свойства" />
        <Tab label="Оценка расстояний" />
        <Tab label="Кластеризация" />
        <Tab label="Распределение степеней" />
        <Tab label="Устойчивость" />
        <Tab label="Время выполнения" />
      </Tabs>
      <Box sx={{ p: 3, mt: 3 }}>
        {tabValue === 0 && <GraphVisualizationComponent datasetname={datasetname!} />}
        {tabValue === 1 && <GeneralPropertiesComponent datasetname={datasetname!} />}
        {tabValue === 2 && <DistanceEstimationComponent datasetname={datasetname!} />}
        {tabValue === 3 && <ClusteringComponent datasetname={datasetname!} />}
        {tabValue === 4 && <DegreeDistributionComponent datasetname={datasetname!} />}
        {tabValue === 5 && <RobustnessComponent datasetname={datasetname!} />}
        {tabValue === 6 && <AlgorithmsRuntimeComponent datasetname={datasetname!} />}
      </Box>
    </Container>
  );
};

export default AnalysisPage;