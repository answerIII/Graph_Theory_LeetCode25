import { decode } from '@msgpack/msgpack';
import type { GraphUpload, MessagePackData, WorkerMessage } from '../types/interfaces';


async function processFile(
    file: File,
    directed: string,
    isVeryLargeGraph: boolean,
    onProgress: (value: number) => void,
    onLog: (message: string) => void
): Promise<GraphUpload> {
    if (directed === '') {
        throw new Error('Выберите тип графа: ориентированный или неориентированный');
    }

    const isCSV = file.name.endsWith('.csv');
    const isMTX = file.name.endsWith('.mtx');
    const isTXT = file.name.endsWith('.txt');
    const isMsgPack = file.name.endsWith('.msgpack');
    if (!isCSV && !isMTX && !isTXT && !isMsgPack) {
        throw new Error('Неподдерживаемый формат: используйте .csv, .txt, .mtx или .msgpack');
    }

    const vertices = new Set<number>();
    let maxVertexId = 0;
    let edgeCount = 0;
    let inputEdgeCount = 0;
    let addedReverseEdges = 0;
    let skippedLines = 0;
    const edges: [number, number][] = [];
    const edgeHashes = new Set<string>(); // Для very_large_graphs
    const stream = file.stream();
    const reader = stream.getReader();
    const decoder = new TextDecoder('utf-8');
    let buffer = '';
    const totalSize = file.size;
    let processedBytes = 0;
    let headerSkipped = false;
    const isCaCoauthors = file.name === 'ca-coauthors-dblp.txt';
    let firstLineSkipped = false;

    if (isMsgPack) {
        // Обработка .msgpack
        const arrayBuffer = await file.arrayBuffer();
        processedBytes = arrayBuffer.byteLength;
        onProgress(100);

        try {
            const data = decode(arrayBuffer) as unknown;
            if (Array.isArray(data)) {
                let metadata: MessagePackData | null = null;
                for (const item of data) {
                    if (!metadata && typeof item === 'object' && 'is_directed' in item) {
                        metadata = item as MessagePackData;
                    } else if (Array.isArray(item) && item.length === 2) {
                        const [u, v] = item.map(Number);
                        if (!isNaN(u) && !isNaN(v)) {
                            vertices.add(u);
                            vertices.add(v);
                            maxVertexId = Math.max(maxVertexId, u, v);
                            if (isVeryLargeGraph && directed === 'false') {
                                inputEdgeCount++;
                                const edgeKey = [Math.min(u, v), Math.max(u, v)].join(':');
                                if (!edgeHashes.has(edgeKey)) {
                                    edgeHashes.add(edgeKey);
                                    edges.push([u, v]);
                                    if (u !== v) {
                                        edges.push([v, u]);
                                        addedReverseEdges++;
                                    }
                                    edgeCount += u !== v ? 2 : 1;
                                }
                            } else {
                                edges.push([u, v]);
                                edgeCount++;
                            }
                        } else {
                            skippedLines++;
                            onLog(`Пропущено некорректное ребро: ${JSON.stringify(item)}`);
                        }
                    }
                }
                if (!metadata) {
                    throw new Error('Отсутствуют метаданные в .msgpack');
                }
                const graph: GraphUpload = {
                    directed: metadata.is_directed,
                    numNodes: metadata.nodeCount,
                    edgeCount,
                    maxVertexId,
                    edges,
                };
                onLog(`Пропущено строк: ${skippedLines}`);
                if (isVeryLargeGraph) {
                    onLog(`Исходных рёбер: ${inputEdgeCount}, Добавлено обратных: ${addedReverseEdges}`);
                }
                return graph;
            } else if (typeof data === 'object' && data !== null && 'is_directed' in data) {
                const msgData = data as MessagePackData;
                const graph: GraphUpload = {
                    directed: msgData.is_directed,
                    numNodes: msgData.nodeCount,
                    edgeCount: msgData.edgeCount ?? 0,
                    maxVertexId,
                    edges: msgData.edges ?? [],
                };
                for (const [u, v] of graph.edges) {
                    vertices.add(u);
                    vertices.add(v);
                    maxVertexId = Math.max(maxVertexId, u, v);
                }
                if (vertices.size !== msgData.nodeCount) {
                    onLog(`Предупреждение: nodeCount (${msgData.nodeCount}) не совпадает с уникальными вершинами (${vertices.size})`);
                }
                return graph;
            } else {
                throw new Error('Некорректный формат .msgpack');
            }
        } catch (err) {
            throw new Error(`Ошибка декодирования .msgpack: ${(err as Error).message}`);
        }
    }

    // Обработка .csv, .txt, .mtx
    let isCsvHeader = false;
    if (isCSV) {
        const text = await file.slice(0, 1024).text();
        const firstLine = text.split('\n')[0].trim();
        isCsvHeader = firstLine.includes('u') || firstLine.includes('id_1');
    }

    while (true) {
        const { done, value } = await reader.read();
        if (done) break;

        processedBytes += value.length;
        onProgress(Math.min(100, Math.round((processedBytes / totalSize) * 100)));

        buffer += decoder.decode(value, { stream: true });
        const lines = buffer.split('\n');
        buffer = lines.pop() || '';

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i].trim();
            if (line === '' || line.startsWith('#') || line.startsWith('%')) {
                continue;
            }

            if (isCaCoauthors && !firstLineSkipped) {
                firstLineSkipped = true;
                continue;
            }

            if (isCsvHeader && i === 0 && isCSV) {
                continue;
            }

            if (isMTX && !headerSkipped) {
                const parts = line.split(/\s+/);
                if (parts.length >= 3 && !isNaN(parseInt(parts[0], 10))) {
                    headerSkipped = true;
                }
                continue;
            }

            let from: number | undefined, to: number | undefined;
            if (isCSV) {
                const parts = line.split(',');
                if (parts.length >= 2) {
                    from = parseInt(parts[isCsvHeader && parts.includes('id_1') ? parts.indexOf('id_1') : 0], 10);
                    to = parseInt(parts[isCsvHeader && parts.includes('id_2') ? parts.indexOf('id_2') : 1], 10);
                }
            } else {
                const parts = line.split(/\s+/);
                if (parts.length >= 2) {
                    from = parseInt(parts[0], 10);
                    to = parseInt(parts[1], 10);
                }
            }

            if (from !== undefined && to !== undefined && !isNaN(from) && !isNaN(to)) {
                vertices.add(from);
                vertices.add(to);
                maxVertexId = Math.max(maxVertexId, from, to);
                if (isVeryLargeGraph && directed === 'false') {
                    inputEdgeCount++;
                    const edgeKey = [Math.min(from, to), Math.max(from, to)].join(':');
                    if (!edgeHashes.has(edgeKey)) {
                        edgeHashes.add(edgeKey);
                        edges.push([from, to]);
                        if (from !== to) {
                            edges.push([to, from]);
                            addedReverseEdges++;
                        }
                        edgeCount += from !== to ? 2 : 1;
                    }
                } else {
                    edges.push([from, to]);
                    edgeCount++;
                }
            } else {
                skippedLines++;
                onLog(`Пропущена строка: ${line}`);
            }
        }
    }

    if (buffer.trim()) {
        const parts = buffer.trim().split(isCSV ? ',' : /\s+/);
        if (parts.length >= 2) {
            const from = parseInt(parts[0], 10);
            const to = parseInt(parts[1], 10);
            if (!isNaN(from) && !isNaN(to)) {
                vertices.add(from);
                vertices.add(to);
                maxVertexId = Math.max(maxVertexId, from, to);
                if (isVeryLargeGraph && directed === 'false') {
                    inputEdgeCount++;
                    const edgeKey = [Math.min(from, to), Math.max(from, to)].join(':');
                    if (!edgeHashes.has(edgeKey)) {
                        edgeHashes.add(edgeKey);
                        edges.push([from, to]);
                        if (from !== to) {
                            edges.push([to, from]);
                            addedReverseEdges++;
                        }
                        edgeCount += from !== to ? 2 : 1;
                    }
                } else {
                    edges.push([from, to]);
                    edgeCount++;
                }
            }
        }
    }

    if (edges.length === 0) {
        throw new Error('Файл не содержит валидных рёбер');
    }

    onLog(`Пропущено строк: ${skippedLines}`);
    if (isVeryLargeGraph) {
        onLog(`Исходных рёбер: ${inputEdgeCount}, Добавлено обратных: ${addedReverseEdges}`);
    }

    return {
        directed: directed === 'true',
        numNodes: vertices.size,
        edgeCount,
        maxVertexId,
        edges,
    };
}

self.onmessage = async (e: MessageEvent<WorkerMessage>) => {
    const { file, directed, isVeryLargeGraph = false } = e.data;
    try {
        const graph = await processFile(
            file,
            directed,
            isVeryLargeGraph,
            (progress: number) => {
                self.postMessage({ type: 'progress', progress });
            },
            (message: string) => {
                self.postMessage({ type: 'log', log: message });
            }
        );
        self.postMessage({ type: 'result', graph });
    } catch (err: unknown) {
        self.postMessage({
            type: 'error',
            error: (err as Error).message || 'Ошибка обработки файла',
        });
    }
};