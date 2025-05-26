"""
Скрипт для обработки графовых датасетов (.mtx, .txt, .csv) с надстройкой для very_large_graphs.

Назначение:
- Читает графы из файлов форматов .txt, .mtx, .csv.
- Для very_large_graphs: добавляет обратное ребро B->A для каждого A->B, пропускает дубликаты.
- Для directed/undirected: сохраняет рёбра как есть, включая петли.
- Определяет is_directed: true для 'directed', false для 'undirected' и 'very_large_graphs'.
- Сохраняет результат в .msgpack, .json, .csv.
- Выводит is_directed (true/false) в консоль.
- Оптимизирован для больших графов (например, orkut.txt с 117M+ рёбер).

Поддерживаемые входные форматы:
- .txt: Пары вершин (FromNodeId ToNodeId), комментарии с `#` (например, CA-GrQc.txt).
- .mtx: MatrixMarket формат, пропускаются строки с `%` (например, soc-wiki-Vote.mtx).
- .csv: Столбцы `u,v` или `id_1,id_2`, игнорируются лишние столбцы (например, vk.csv).

Структура выходных файлов:
{
  "is_directed": boolean,
  "nodeCount": number,
  "edgeCount": number,
  "edges": [[u, v], ...]
}

Зависимости:
- Установите: `pip install pandas msgpack tqdm`

Аргументы:
- `-i/--input <путь>`: Путь к входному файлу.
- `-o/--output <префикс>`: Префикс для выходных файлов.
- `--output-formats <msgpack|json|csv>`: Форматы (default: msgpack).
- `--no-progress`: Отключает прогресс-бар.

Примеры:
1. ```bash
   python process_graph.py -i undirected/CA-GrQc.txt -o output --output-formats json
   ```
2. ```bash
   python process_graph.py -i very_large_graphs/vk.csv -o output --output-formats json
   ```

Особенности:
- Потоковая обработка, RAM ~1 ГБ.
- Для very_large_graphs: добавляет B->A, пропускает дубликаты через set с хешами.
- Для .csv: nodeCount = число уникальных вершин (len(vertices)).
- Логирует пропущенные строки, реальное число вершин и исходное число рёбер.
"""

import pandas as pd
import json
import csv
import msgpack
import os
import sys
import argparse
import tempfile
import re
from pathlib import Path
from tqdm import tqdm

def parse_header(file_path):
    """Парсит заголовок для числа вершин и рёбер."""
    node_count = None
    edge_count = None
    extension = Path(file_path).suffix.lower()

    with open(file_path, 'r', encoding='utf-8') as f:
        if extension == '.txt':
            for line in f:
                line = line.strip()
                if line.startswith('#'):
                    match = re.search(r'#.*Nodes:\s*(\d+)\s*Edges:\s*(\d+)', line)
                    if match:
                        node_count = int(match.group(1))
                        edge_count = int(match.group(2))
                        break
                elif line:
                    break
        elif extension == '.mtx':
            for line in f:
                line = line.strip()
                if not line.startswith('%') and line:
                    parts = line.split()
                    if len(parts) >= 3:
                        node_count = int(parts[0])
                        edge_count = int(parts[2])
                        break
        # .csv не содержит nodeCount/edgeCount в заголовке

    return node_count, edge_count

def is_directed_graph(file_path):
    """Определяет is_directed по папке."""
    file_path = str(file_path).lower()
    return 'directed' in file_path and 'undirected' not in file_path

def is_very_large_graph(file_path):
    """Проверяет, находится ли файл в very_large_graphs."""
    file_path = str(file_path).lower()
    return 'very_large_graphs' in file_path

def process_graph_streaming(file_path, output_prefix, formats, progress=True):
    """Обрабатывает граф, с надстройкой для very_large_graphs."""
    file_path = Path(file_path)
    if not file_path.exists():
        print(f"Файл {file_path} не найден")
        sys.exit(1)

    vertices = set()
    edge_count = 0
    input_edge_count = 0
    skipped_lines = 0
    skipped_examples = []
    added_reverse_edges = 0
    extension = file_path.suffix.lower()
    temp_file = tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.tmp')

    is_directed = is_directed_graph(file_path)
    is_very_large = is_very_large_graph(file_path)
    header_node_count, header_edge_count = parse_header(file_path)

    edge_hashes = set()  # Для very_large_graphs: храним хеши рёбер

    try:
        # Первый проход: читаем и записываем рёбра во временный файл
        if extension == '.txt':
            total_lines = sum(1 for line in open(file_path, 'r', encoding='utf-8') if line.strip() and not line.startswith('#'))
            is_ca_coauthors = file_path.name == 'ca-coauthors-dblp.txt'
            if is_ca_coauthors:
                total_lines -= 1
            with open(file_path, 'r', encoding='utf-8') as f:
                if is_ca_coauthors:
                    f.readline()
                for line in tqdm(f, total=total_lines, desc="Reading .txt", disable=not progress, mininterval=1.0):
                    line = line.strip()
                    if not line or line.startswith('#'):
                        continue
                    try:
                        parts = re.split(r'\s+', line)
                        if len(parts) >= 2:
                            u, v = map(int, parts[:2])
                            vertices.add(u)
                            vertices.add(v)
                            if is_very_large:
                                input_edge_count += 1
                                edge_key = (min(u, v), max(u, v))
                                edge_hash = hash(edge_key)
                                if edge_hash not in edge_hashes:
                                    edge_hashes.add(edge_hash)
                                    temp_file.write(f"{u}\t{v}\n")
                                    if u != v:
                                        temp_file.write(f"{v}\t{u}\n")
                                        added_reverse_edges += 1
                                    edge_count += 2 if u != v else 1
                            else:
                                temp_file.write(f"{u}\t{v}\n")
                                edge_count += 1
                        else:
                            skipped_lines += 1
                            if len(skipped_examples) < 10:
                                skipped_examples.append(f"Недостаточно данных: {line}")
                    except ValueError:
                        skipped_lines += 1
                        if len(skipped_examples) < 10:
                            skipped_examples.append(f"Ошибка парсинга: {line}")
                        continue

        elif extension == '.mtx':
            total_lines = sum(1 for line in open(file_path, 'r', encoding='utf-8') if line.strip() and not line.startswith('%')) - 1
            with open(file_path, 'r', encoding='utf-8') as f:
                header_passed = False
                for line in tqdm(f, total=total_lines, desc="Reading .mtx", disable=not progress, mininterval=1.0):
                    line = line.strip()
                    if not line or line.startswith('%'):
                        continue
                    if not header_passed:
                        header_passed = True
                        continue
                    try:
                        parts = re.split(r'\s+', line)
                        if len(parts) >= 2:
                            u, v = map(int, parts[:2])
                            vertices.add(u)
                            vertices.add(v)
                            if is_very_large:
                                input_edge_count += 1
                                edge_key = (min(u, v), max(u, v))
                                edge_hash = hash(edge_key)
                                if edge_hash not in edge_hashes:
                                    edge_hashes.add(edge_hash)
                                    temp_file.write(f"{u}\t{v}\n")
                                    if u != v:
                                        temp_file.write(f"{v}\t{u}\n")
                                        added_reverse_edges += 1
                                    edge_count += 2 if u != v else 1
                            else:
                                temp_file.write(f"{u}\t{v}\n")
                                edge_count += 1
                        else:
                            skipped_lines += 1
                            if len(skipped_examples) < 10:
                                skipped_examples.append(f"Недостаточно данных: {line}")
                    except ValueError:
                        skipped_lines += 1
                        if len(skipped_examples) < 10:
                            skipped_examples.append(f"Ошибка парсинга: {line}")
                        continue

        elif extension == '.csv':
            with open(file_path, 'r', encoding='utf-8') as f:
                first_line = f.readline().strip()
                has_header = 'u' in first_line or 'id_1' in first_line
            df = pd.read_csv(file_path, comment='#', iterator=True, chunksize=10000, header=0 if has_header else None)
            total_lines = sum(len(chunk) for chunk in pd.read_csv(file_path, comment='#', chunksize=10000, header=0 if has_header else None))
            for chunk in tqdm(df, total=total_lines//10000 + 1, desc="Reading .csv", disable=not progress, mininterval=1.0):
                if has_header:
                    if 'u' in chunk.columns and 'v' in chunk.columns:
                        edges_data = chunk[['u', 'v']].values
                    elif 'id_1' in chunk.columns and 'id_2' in chunk.columns:
                        edges_data = chunk[['id_1', 'id_2']].values
                    else:
                        print(f"Неизвестный формат CSV в {file_path}: ожидаются u,v или id_1,id_2")
                        sys.exit(1)
                else:
                    edges_data = chunk.iloc[:, :2].values
                for u, v in edges_data:
                    try:
                        u, v = int(u), int(v)
                        vertices.add(u)
                        vertices.add(v)
                        if is_very_large:
                            input_edge_count += 1
                            edge_key = (min(u, v), max(u, v))
                            edge_hash = hash(edge_key)
                            if edge_hash not in edge_hashes:
                                edge_hashes.add(edge_hash)
                                temp_file.write(f"{u}\t{v}\n")
                                if u != v:
                                    temp_file.write(f"{v}\t{u}\n")
                                    added_reverse_edges += 1
                                edge_count += 2 if u != v else 1
                        else:
                            temp_file.write(f"{u}\t{v}\n")
                            edge_count += 1
                    except ValueError:
                        skipped_lines += 1
                        if len(skipped_examples) < 10:
                            skipped_examples.append(f"Ошибка парсинга: {u}, {v}")
                        continue

        else:
            print(f"Неподдерживаемый формат файла: {extension}")
            sys.exit(1)

        temp_file.close()

        # Устанавливаем nodeCount
        if header_node_count is not None:
            node_count = header_node_count
        else:
            node_count = len(vertices) if vertices else 0  # Используем len(vertices) для .csv

        # Проверяем edgeCount для directed/undirected
        if not is_very_large and header_edge_count is not None and edge_count != header_edge_count:
            print(f"Предупреждение: edgeCount ({edge_count}) не совпадает с заголовком ({header_edge_count}) в {file_path}")

        # Логируем статистику
        print(f"Граф обработан из {file_path}:")
        print(f"  nodeCount: {node_count}")
        print(f"  Уникальных вершин: {len(vertices)}")
        print(f"  Максимальный ID вершины: {max(vertices) if vertices else 0}")
        print(f"  edgeCount: {edge_count}")
        if is_very_large:
            print(f"  Исходных рёбер: {input_edge_count}")
            print(f"  Добавлено обратных рёбер: {added_reverse_edges}")
        if skipped_lines > 0:
            print(f"Пропущено строк: {skipped_lines}")
            for example in skipped_examples:
                print(example)

        # Второй проход: формируем выходные файлы
        for fmt in formats:
            output_path = Path(f"{output_prefix}.{fmt}")
            try:
                if fmt == 'msgpack':
                    with open(temp_file.name, 'r') as tf, open(output_path, 'wb') as f:
                        # Собираем все рёбра в список
                        edges = []
                        for line in tqdm(tf, total=edge_count, desc="Collecting edges for .msgpack", disable=not progress, mininterval=1.0):
                            u, v = map(int, line.strip().split('\t'))
                            edges.append([u, v])
                        # Записываем один объект
                        data = {
                            "is_directed": False if is_very_large else is_directed,
                            "nodeCount": node_count,
                            "edgeCount": edge_count,
                            "edges": edges
                        }
                        f.write(msgpack.packb(data))

                elif fmt == 'json':
                    with open(temp_file.name, 'r') as tf, open(output_path, 'w', encoding='utf-8') as f:
                        f.write('{\n  "is_directed": %s,\n  "nodeCount": %d,\n  "edgeCount": %d,\n  "edges": [\n' % (str(False if is_very_large else is_directed).lower(), node_count, edge_count))
                        first = True
                        for line in tqdm(tf, total=edge_count, desc="Writing .json", disable=not progress, mininterval=1.0):
                            u, v = map(int, line.strip().split('\t'))
                            if not first:
                                f.write(',\n')
                            f.write(f'    [%d, %d]' % (u, v))
                            first = False
                        f.write('\n  ]\n}')

                elif fmt == 'csv':
                    with open(temp_file.name, 'r') as tf, open(output_path, 'w', newline='') as f:
                        writer = csv.writer(f)
                        writer.writerow(['is_directed', False if is_very_large else is_directed])
                        writer.writerow(['nodeCount', node_count])
                        writer.writerow(['edgeCount', edge_count])
                        writer.writerow(['source', 'target'])
                        for line in tqdm(tf, total=edge_count, desc="Writing .csv", disable=not progress, mininterval=1.0):
                            u, v = map(int, line.strip().split('\t'))
                            writer.writerow([u, v])

                print(f"Граф сохранён в {output_path}")
            except Exception as e:
                print(f"Ошибка при записи {output_path}: {e}")
                sys.exit(1)

        # Выводим is_directed
        print(False if is_very_large else is_directed)

    except Exception as e:
        print(f"Ошибка при обработке {file_path}: {e}")
        sys.exit(1)
    finally:
        os.unlink(temp_file.name)

def main():
    parser = argparse.ArgumentParser(description="Process graph input files (.txt, .mtx, .csv).")
    parser.add_argument('-i', '--input', type=str, required=True, help='Path to input graph file')
    parser.add_argument('-o', '--output', type=str, required=True, help='Output file prefix')
    parser.add_argument('--output-formats', type=str, nargs='+', choices=['msgpack', 'json', 'csv'], default=['msgpack'],
                        help='Output formats (default: msgpack)')
    parser.add_argument('--no-progress', action='store_true', help='Disable progress bars')

    args = parser.parse_args()

    process_graph_streaming(args.input, args.output, args.output_formats, progress=not args.no_progress)

if __name__ == "__main__":
    main()