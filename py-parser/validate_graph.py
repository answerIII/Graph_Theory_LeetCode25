"""
Скрипт для валидации графовых датасетов (.csv, .txt, .mtx, .msgpack).

Назначение:
- Подсчитывает уникальные вершины, максимальный ID вершины и количество рёбер.
- Для .msgpack: проверяет is_directed, nodeCount, edgeCount.
- Поддерживает форматы:
  - .csv: Столбцы u,v или id_1,id_2, игнорирует заголовок и лишние столбцы.
  - .txt: Пары вершин, пропускает комментарии с #.
  - .mtx: MatrixMarket, пропускает строки с %, игнорирует заголовок.
  - .msgpack: Выходной формат process_graph.py (один объект с edges).
- Оптимизирован для больших файлов (например, orkut.txt с 117M+ рёбер).

Вывод:
- Уникальных вершин (len(set(u) ∪ set(v))).
- Максимальный ID вершины (max(vertices)).
- Исходных рёбер (число строк с рёбрами).
- Для .msgpack: is_directed, nodeCount, edgeCount из файла.

Зависимости:
- Установите: `pip install pandas msgpack tqdm`

Аргументы:
- `-i/--input <путь>`: Путь к файлу.
- `--no-progress`: Отключает прогресс-бар.

Примеры:
1. ```bash
   python validate_graph.py -i very_large_graphs/vk.csv
   ```
2. ```bash
   python validate_graph.py -i output.msgpack --no-progress
   ```

Особенности:
- Потоковая обработка, RAM ~100-200 МБ.
- Поддерживает ca-coauthors-dblp.txt (пропуск первой строки).
- Логирует пропущенные строки и расхождения в .msgpack.
"""

import pandas as pd
import msgpack
import argparse
import re
from pathlib import Path
from tqdm import tqdm

def validate_graph(file_path, progress=True):
    """Валидирует граф, подсчитывая вершины и рёбра."""
    file_path = Path(file_path)
    if not file_path.exists():
        print(f"Файл {file_path} не найден")
        return

    vertices = set()
    max_vertex = 0
    edge_count = 0
    skipped_lines = 0
    skipped_examples = []
    extension = file_path.suffix.lower()

    try:
        if extension == '.csv':
            with open(file_path, 'r', encoding='utf-8') as f:
                first_line = f.readline().strip()
                has_header = 'u' in first_line or 'id_1' in first_line
            df = pd.read_csv(file_path, comment='#', iterator=True, chunksize=10000, header=0 if has_header else None)
            total_lines = sum(len(chunk) for chunk in pd.read_csv(file_path, comment='#', chunksize=10000, header=0 if has_header else None))
            for chunk in tqdm(df, total=total_lines//10000 + 1, desc="Validating .csv", disable=not progress, mininterval=1.0):
                if has_header:
                    if 'u' in chunk.columns and 'v' in chunk.columns:
                        edges_data = chunk[['u', 'v']].values
                    elif 'id_1' in chunk.columns and 'id_2' in chunk.columns:
                        edges_data = chunk[['id_1', 'id_2']].values
                    else:
                        print(f"Неизвестный формат CSV: ожидаются u,v или id_1,id_2")
                        return
                else:
                    edges_data = chunk.iloc[:, :2].values
                for u, v in edges_data:
                    try:
                        u, v = int(u), int(v)
                        vertices.add(u)
                        vertices.add(v)
                        max_vertex = max(max_vertex, u, v)
                        edge_count += 1
                    except ValueError:
                        skipped_lines += 1
                        if len(skipped_examples) < 10:
                            skipped_examples.append(f"Ошибка парсинга: {u}, {v}")

        elif extension == '.txt':
            total_lines = sum(1 for line in open(file_path, 'r', encoding='utf-8') if line.strip() and not line.startswith('#'))
            is_ca_coauthors = file_path.name == 'ca-coauthors-dblp.txt'
            if is_ca_coauthors:
                total_lines -= 1
            with open(file_path, 'r', encoding='utf-8') as f:
                if is_ca_coauthors:
                    f.readline()
                for line in tqdm(f, total=total_lines, desc="Validating .txt", disable=not progress, mininterval=1.0):
                    line = line.strip()
                    if not line or line.startswith('#'):
                        continue
                    try:
                        parts = re.split(r'\s+', line)
                        if len(parts) >= 2:
                            u, v = map(int, parts[:2])
                            vertices.add(u)
                            vertices.add(v)
                            max_vertex = max(max_vertex, u, v)
                            edge_count += 1
                        else:
                            skipped_lines += 1
                            if len(skipped_examples) < 10:
                                skipped_examples.append(f"Недостаточно данных: {line}")
                    except ValueError:
                        skipped_lines += 1
                        if len(skipped_examples) < 10:
                            skipped_examples.append(f"Ошибка парсинга: {line}")

        elif extension == '.mtx':
            total_lines = sum(1 for line in open(file_path, 'r', encoding='utf-8') if line.strip() and not line.startswith('%')) - 1
            with open(file_path, 'r', encoding='utf-8') as f:
                header_passed = False
                for line in tqdm(f, total=total_lines, desc="Validating .mtx", disable=not progress, mininterval=1.0):
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
                            max_vertex = max(max_vertex, u, v)
                            edge_count += 1
                        else:
                            skipped_lines += 1
                            if len(skipped_examples) < 10:
                                skipped_examples.append(f"Недостаточно данных: {line}")
                    except ValueError:
                        skipped_lines += 1
                        if len(skipped_examples) < 10:
                            skipped_examples.append(f"Ошибка парсинга: {line}")

        elif extension == '.msgpack':
            with open(file_path, 'rb') as f:
                # Читаем весь объект
                data = msgpack.unpackb(f.read(), raw=False)
                if not isinstance(data, dict):
                    print("Ошибка: некорректный формат .msgpack (ожидается словарь)")
                    return
                is_directed = data.get('is_directed', None)
                node_count = data.get('nodeCount', None)
                edge_count_expected = data.get('edgeCount', None)
                edges = data.get('edges', None)
                if None in (is_directed, node_count, edge_count_expected, edges):
                    print("Ошибка: отсутствуют is_directed, nodeCount, edgeCount или edges в .msgpack")
                    return

                # Обрабатываем рёбра
                for item in tqdm(edges, total=edge_count_expected, desc="Validating .msgpack", disable=not progress, mininterval=1.0):
                    try:
                        if isinstance(item, list) and len(item) == 2:
                            u, v = int(item[0]), int(item[1])
                            vertices.add(u)
                            vertices.add(v)
                            max_vertex = max(max_vertex, u, v)
                            edge_count += 1
                        else:
                            skipped_lines += 1
                            if len(skipped_examples) < 10:
                                skipped_examples.append(f"Некорректное ребро: {item}")
                    except (ValueError, TypeError):
                        skipped_lines += 1
                        if len(skipped_examples) < 10:
                            skipped_examples.append(f"Ошибка парсинга: {item}")

                # Проверяем edgeCount
                if edge_count != edge_count_expected:
                    print(f"Предупреждение: edgeCount ({edge_count}) не совпадает с ожидаемым ({edge_count_expected})")

        else:
            print(f"Неподдерживаемый формат файла: {extension}")
            return

        # Вывод результатов
        print(f"Валидация {file_path}:")
        if extension == '.msgpack':
            print(f"  is_directed: {is_directed}")
            print(f"  nodeCount (из файла): {node_count}")
            print(f"  edgeCount (из файла): {edge_count_expected}")
        print(f"  Уникальных вершин: {len(vertices)}")
        print(f"  Максимальный ID вершины: {max_vertex}")
        print(f"  Рёбер (подсчитано): {edge_count}")
        if extension == '.msgpack' and len(vertices) != node_count:
            print(f"Предупреждение: nodeCount ({node_count}) не совпадает с уникальными вершинами ({len(vertices)})")
        if skipped_lines > 0:
            print(f"  Пропущено строк: {skipped_lines}")
            for example in skipped_examples:
                print(f"    {example}")

    except Exception as e:
        print(f"Ошибка при валидации {file_path}: {e}")

def main():
    parser = argparse.ArgumentParser(description="Validate graph input files (.csv, .txt, .mtx, .msgpack).")
    parser.add_argument('-i', '--input', type=str, required=True, help='Path to input graph file')
    parser.add_argument('--no-progress', action='store_true', help='Disable progress bars')

    args = parser.parse_args()

    validate_graph(args.input, progress=not args.no_progress)

if __name__ == "__main__":
    main()