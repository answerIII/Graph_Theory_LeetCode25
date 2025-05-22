import React from 'react';
import { Box, Typography, Container } from '@mui/material';
import GraphUploader from '../components/GraphUploader';

const HomePage: React.FC = () => {
  return (
    <Container maxWidth="md" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom align="center">
        Анализ графов
      </Typography>
      <Box sx={{ mb: 4 }}>
        <GraphUploader />
      </Box>
    </Container>
  );
};

export default HomePage;