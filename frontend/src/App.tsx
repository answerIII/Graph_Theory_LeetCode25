import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import HomePage from './pages/HomePage';
import AnalysisPage from './pages/AnalysisPage';
import ComparisonPage from './pages/ComparisonPage';

const App: React.FC = () => {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<HomePage />} />
        <Route path="/graphs/:datasetname" element={<AnalysisPage />} />
        <Route path="/comparison" element={<ComparisonPage />} />
      </Routes>
    </Router>
  );
};

export default App;