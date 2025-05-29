// src/api/graphApi.ts
import axiosInstance from './axiosInstance';
import type {
  //   GraphData,
  GeneralPropertiesData,
  DistanceResultData,
  ClusteringData,
  DegreeDistributionData,
  RobustnessData,
  RuntimeData,
  //   ComparisonData,
} from '../types/graphTypes';

interface RequestBody {
  [key: string]: any;
}

export const graphApi = {
  //   // 1. Получение графа для визуализации
  //   getGraph: async (datasetname: string): Promise<GraphData> => {
  //     const response = await axiosInstance.get(`/graphs/${datasetname}`);
  //     return response.data;
  //   },

  // 2. Получение общих характеристик графа
  getGeneralProperties: async (datasetname: string): Promise<GeneralPropertiesData> => {
    const response = await axiosInstance.get(`/graphs/${datasetname}/properties`);
    return response.data;
  },

  // 3. Оценка расстояний
  getDistances: async (
    datasetname: string,
    method: 'double_sweep' | 'random_sample' | 'snowball',
    sampleSize?: number
  ): Promise<DistanceResultData> => {
    const body: RequestBody = { method };
    if (sampleSize && method !== 'double_sweep') {
      body.sampleSize = sampleSize;
    }
    const response = await axiosInstance.post(`/graphs/${datasetname}/distances`, body);
    return response.data;
  },

  // 4. Получение данных кластеризации
  getClustering: async (datasetname: string): Promise<ClusteringData> => {
    const response = await axiosInstance.get(`/graphs/${datasetname}/clustering`);
    return response.data;
  },

  // 5. Получение распределения степеней
  getDegreeDistribution: async (datasetname: string): Promise<DegreeDistributionData> => {
    const response = await axiosInstance.get(`/graphs/${datasetname}/degree-distribution`);
    return response.data;
  },

  // 6. Анализ устойчивости
  getRobustness: async (
    datasetname: string,
    xPercent: number,
    removalMethod: 'random' | 'targeted'
  ): Promise<RobustnessData[]> => {
    const response = await axiosInstance.post(`/graphs/${datasetname}/robustness`, {
      xPercent,
      removalMethod,
    });
    return response.data;
  },

  // 7. Получение метрик производительности
  getRuntimeMetrics: async (datasetname: string): Promise<RuntimeData[]> => {
    const response = await axiosInstance.get(`/graphs/${datasetname}/metrics`);
    return response.data;
  },

  // 8. Сравнение датасетов
  //   compareDatasets: async (
  //     datasetNames: string[],
  //     metrics: string[]
  //   ): Promise<ComparisonData[]> => {
  //     const response = await axiosInstance.post('/graphs/compare', {
  //       datasetNames,
  //       metrics,
  //     });
  //     return response.data;
  //   },

  //   // 9. Загрузка нового графа
  //   uploadGraph: async (graph: GraphData): Promise<void> => {
  //     try {
  //       await axiosInstance.post('/upload', graph);
  //     } catch (error) {
  //       throw new Error(`Ошибка отправки графа на бэкенд: ${(error as Error).message}`);
  //     }
  //   },

  // 10. Выбор датасета
  selectDataset: async (datasetname: string): Promise<void> => {
    try {
      await axiosInstance.post('/dataset', { datasetname });
    } catch (error) {
      throw new Error(`Ошибка выбора датасета ${datasetname}: ${(error as Error).message}`);
    }
  },

  // 11. Получить результат вычислений расстояний (стандарт)
  getDistanceResult: async (datasetname: string) => {
    const response = await axiosInstance.get(`/graphs/${datasetname}/distance`);
    return response.data;
  },

  // 12. Вычислить расстояние между вершинами
  calculateDistance: async (
    datasetname: string,
    payload: {
      start_node: number;
      end_node: number;
      algorithm: string;
      landmarks: { count: number; selection: string };
    }
  ) => {
    const response = await axiosInstance.post(`/graphs/${datasetname}/distance`, payload);
    return response.data;
  },
};

