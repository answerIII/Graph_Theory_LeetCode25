import { encode } from '@msgpack/msgpack';
import type { Graph } from '../types/graph';

// Скачивает граф в MessagePack с использованием FileSystem API
export async function fileSystemDownloadGraph(graphInfo: Graph, fileName: string): Promise<void> {
    try {
        if (!graphInfo.edgesBuffer || graphInfo.edgesBuffer.byteLength === 0) {
            throw new Error('edgesBuffer отсутствует или пустой');
        }

        // Запрашиваем доступ к файловой системе
        const handle = await window.showSaveFilePicker({
            suggestedName: `processed_${fileName}.msgpack`,
            types: [{ description: 'MessagePack', accept: { 'application/octet-stream': ['.msgpack'] } }],
        });

        const writable = await handle.createWritable();
        const buffer = new Uint8Array(graphInfo.edgesBuffer);
        const decoder = new TextDecoder('utf-8');
        const chunkSize = 1024 * 1024; // 1 МБ
        let offset = 0;
        let edges: { from: number; to: number }[] = [];
        let remainingText = '';

        while (offset < buffer.length) {
            const chunk = buffer.slice(offset, offset + chunkSize);
            const text = decoder.decode(chunk, { stream: true });
            const lines = (remainingText + text).split('\n');
            remainingText = lines.pop() || '';

            for (const line of lines) {
                if (line.trim()) {
                    const [from, to] = line.split(',').map(Number);
                    if (!isNaN(from) && !isNaN(to)) {
                        edges.push({ from, to });
                    }
                }
            }

            if (edges.length >= 100_000) {
                const formattedChunk = { edges };
                await writable.write(encode(formattedChunk));
                edges = [];
            }

            offset += chunkSize;
        }

        if (remainingText.trim()) {
            const [from, to] = remainingText.split(',').map(Number);
            if (!isNaN(from) && !isNaN(to)) {
                edges.push({ from, to });
            }
        }

        // Записываем метаданные и оставшиеся рёбра
        const formattedGraph = {
            directed: graphInfo.directed,
            vertexCount: graphInfo.numNodes,
            numEdges: graphInfo.numEdges,
            edges,
        };
        await writable.write(encode(formattedGraph));
        await writable.close();
    } catch (err: unknown) {
        console.error('fileSystemDownloadGraph: Ошибка:', err);
        throw new Error(`Ошибка при скачивании через FileSystem API: ${(err as Error).message || 'Неизвестная ошибка'}`);
    }
}