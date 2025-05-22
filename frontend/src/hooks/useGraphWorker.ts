import { useState, useCallback } from 'react';
import { throttle } from 'lodash';
import type { Graph } from '../types/graph';

interface WorkerResponse {
    type: 'result' | 'progress' | 'error';
    graph?: Graph;
    progress?: number;
    error?: string;
}

export function useGraphWorker() {
    const [progress, setProgress] = useState<number>(0);
    const [loading, setLoading] = useState<boolean>(false);
    const [error, setError] = useState<string | null>(null);
    const [graph, setGraph] = useState<Graph | null>(null);

    const throttledSetProgress = useCallback(
        throttle((value: number) => setProgress(value), 100),
        []
    );

    const processFile = useCallback(async (file: File, directed: string) => {
        if (file.size > 1024 * 1024 * 1024) {
            throw new Error('Файл слишком большой (максимум 1 ГБ)');
        }

        setLoading(true);
        setError(null);
        setProgress(0);
        setGraph(null);

        const worker = new Worker(new URL('../workers/graphWorker.ts', import.meta.url), {
            type: 'module',
        });

        return new Promise<Graph>((resolve, reject) => {
            worker.onmessage = (e: MessageEvent<WorkerResponse>) => {
                if (e.data.type === 'result' && e.data.graph) {
                    setGraph(e.data.graph);
                    resolve(e.data.graph);
                } else if (e.data.type === 'progress' && e.data.progress !== undefined) {
                    throttledSetProgress(e.data.progress);
                } else if (e.data.type === 'error' && e.data.error) {
                    setError(e.data.error);
                    reject(new Error(e.data.error));
                }
            };

            worker.onerror = (err) => {
                setError(`Ошибка в Worker: ${err.message}`);
                reject(new Error(err.message));
            };

            worker.postMessage({ file, directed });
        }).finally(() => {
            setLoading(false);
            worker.terminate();
        });
    }, [throttledSetProgress]);

    return { processFile, progress, loading, error, graph };
}