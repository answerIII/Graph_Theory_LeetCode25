import { encode } from '@msgpack/msgpack';
import type { GraphUpload } from '../types/interfaces';

export async function downloadGraph(
    graphInfo: GraphUpload,
    fileName: string,
    downloadFormat: 'json' | 'csv' | 'msgpack'
): Promise<void> {
    const safeFileName = fileName || 'graph';
    const baseName = safeFileName.includes('.') ? safeFileName.split('.').slice(0, -1).join('') : safeFileName;
    const processedFileName = `processed_${baseName}`;
    const extension = downloadFormat === 'msgpack' ? 'msgpack' : downloadFormat;

    const handle = await window.showSaveFilePicker({
        suggestedName: `${processedFileName}.${extension}`,
        types: [
            {
                description: downloadFormat.toUpperCase(),
                accept: {
                    [downloadFormat === 'json' ? 'application/json' : downloadFormat === 'csv' ? 'text/csv' : 'application/octet-stream']: [`.${extension}`],
                },
            },
        ],
    });

    const writable = await handle.createWritable();
    const chunkSize = 100_000; // 100K рёбер за чанк

    if (downloadFormat === 'json') {
        const header = `{\n  "is_directed": ${graphInfo.is_directed},\n  "nodeCount": ${graphInfo.numNodes},\n  "edgeCount": ${graphInfo.edgeCount},\n  "edges": [\n`;
        await writable.write(header);

        for (let i = 0; i < graphInfo.edges.length; i += chunkSize) {
            const chunk = graphInfo.edges.slice(i, i + chunkSize);
            const chunkText = chunk.map(([u, v]) => `    [${u}, ${v}]`).join(',\n');
            const prefix = i === 0 ? '' : ',\n';
            await writable.write(prefix + chunkText);
        }

        await writable.write('\n  ]\n}');
    } else if (downloadFormat === 'csv') {
        const header = `is_directed,${graphInfo.is_directed}\nnodeCount,${graphInfo.numNodes}\nedgeCount,${graphInfo.edgeCount}\nsource,target\n`;
        await writable.write(header);

        for (let i = 0; i < graphInfo.edges.length; i += chunkSize) {
            const chunk = graphInfo.edges.slice(i, i + chunkSize);
            const chunkText = chunk.map(([u, v]) => `${u},${v}`).join('\n');
            await writable.write(chunkText + (i + chunkSize < graphInfo.edges.length ? '\n' : ''));
        }
    } else {
        const formattedGraph = {
            is_directed: graphInfo.is_directed,
            nodeCount: graphInfo.numNodes,
            edgeCount: graphInfo.edgeCount,
            edges: graphInfo.edges,
        };
        const data = encode(formattedGraph);
        await writable.write(data);
    }

    await writable.close();
}