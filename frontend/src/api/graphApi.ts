import axios from 'axios';
import type { Graph } from '../types/graph';

const API_URL = 'http://localhost:8085';

export const uploadGraph = async (graph: Graph): Promise<void> => {

    try{
        console.log(graph);
        await axios.post(`${API_URL}/upload`, graph, {
            headers: {'Content-Type' : 'application/json'},
        });
    }
    catch(error){
        throw new Error('Ошибка отправки графа на бэкенд' + (error as Error).message);
    }
}