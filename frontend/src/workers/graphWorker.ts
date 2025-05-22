import type { Graph } from '../types/graph';

interface WorkerMessage {
    file: File;
    directed: string;
}

async function processFile(
    file: File,
    directed: string,
    onProgress: (value: number) => void
): Promise<Graph> {
    if (directed === '') {
        throw new Error('Выберите тип графа: ориентированный или неориентированный');
    }

    const isCSV = file.name.endsWith('.csv');
    const isMTX = file.name.endsWith('.mtx');
    const isTXT = file.name.endsWith('.txt');
    if (!isCSV && !isMTX && !isTXT) {
        throw new Error('Неподдерживаемый формат: используйте .csv, .txt или .mtx');
    }

    const nodeMap = new Map<number, number>();
    let nextNodeId = 0;
    let headerSkipped = false;
    const edges: [number, number][] = [];
    const stream = file.stream();
    const reader = stream.getReader();
    const decoder = new TextDecoder('utf-8');
    let buffer = '';
    const totalSize = file.size;
    let processedBytes = 0;

    while (true) {
        const { done, value } = await reader.read();
        if (done) break;

        processedBytes += value.length;
        onProgress(Math.min(100, Math.round((processedBytes / totalSize) * 100)));

        buffer += decoder.decode(value, { stream: true });
        const lines = buffer.split('\n');
        buffer = lines.pop() || '';

        for (const line of lines) {
            const trimmedLine = line.trim();
            if (trimmedLine === '' || trimmedLine.startsWith('#') || trimmedLine.startsWith('%')) {
                continue;
            }

            let from: number | undefined, to: number | undefined;
            if (isCSV) {
                const parts = trimmedLine.split(',');
                if (parts.length >= 2) {
                    from = parseInt(parts[0], 10);
                    to = parseInt(parts[1], 10);
                }
            } else if (isMTX) {
                if (!headerSkipped) {
                    const parts = trimmedLine.split(/\s+/);
                    if (parts.length >= 3 && !isNaN(parseInt(parts[0], 10))) {
                        headerSkipped = true;
                    }
                    continue;
                } else {
                    const parts = trimmedLine.split(/\s+/);
                    if (parts.length >= 2) {
                        from = parseInt(parts[0], 10);
                        to = parseInt(parts[1], 10);
                    }
                }
            } else {
                const parts = trimmedLine.split(/\s+/);
                if (parts.length >= 2) {
                    from = parseInt(parts[0], 10);
                    to = parseInt(parts[1], 10);
                }
            }

            if (from !== undefined && to !== undefined && !isNaN(from) && !isNaN(to)) {
                if (!nodeMap.has(from)) nodeMap.set(from, nextNodeId++);
                if (!nodeMap.has(to)) nodeMap.set(to, nextNodeId++);
                edges.push([nodeMap.get(from)!, nodeMap.get(to)!]);
            }
        }
    }

    if (buffer.trim()) {
        const parts = buffer.trim().split(isCSV ? ',' : /\s+/);
        if (parts.length >= 2) {
            const from = parseInt(parts[0], 10);
            const to = parseInt(parts[1], 10);
            if (!isNaN(from) && !isNaN(to)) {
                if (!nodeMap.has(from)) nodeMap.set(from, nextNodeId++);
                if (!nodeMap.has(to)) nodeMap.set(to, nextNodeId++);
                edges.push([nodeMap.get(from)!, nodeMap.get(to)!]);
            }
        }
    }

    if (edges.length === 0) {
        throw new Error('Файл не содержит валидных рёбер');
    }

    return {
        directed: directed === 'true',
        numNodes: nodeMap.size,
        edges,
    };
}

self.onmessage = async (e: MessageEvent<WorkerMessage>) => {
    const { file, directed } = e.data;
    try {
        const graph = await processFile(file, directed, (progress: number) => {
            self.postMessage({ type: 'progress', progress });
        });
        self.postMessage({ type: 'result', graph });
    } catch (err: unknown) {
        self.postMessage({
            type: 'error',
            error: (err as Error).message || 'Ошибка обработки файла',
        });
    }
};