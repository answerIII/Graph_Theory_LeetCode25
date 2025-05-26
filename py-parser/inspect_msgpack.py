"""
Скрипт для анализа содержимого .msgpack файла.

Назначение:
- Читает .msgpack и выводит первые несколько объектов.
- Помогает понять структуру данных.

Зависимости:
- pip install msgpack

Использование:
```bash
python inspect_msgpack.py -i output.msgpack
```
"""

import msgpack
import argparse
from pathlib import Path

def inspect_msgpack(file_path):
    file_path = Path(file_path)
    if not file_path.exists():
        print(f"Файл {file_path} не найден")
        return

    try:
        with open(file_path, 'rb') as f:
            unpacker = msgpack.Unpacker(f, raw=False)
            print(f"Содержимое {file_path}:")
            for i, item in enumerate(unpacker):
                print(f"Объект {i + 1}: {item}")
                if i >= 4:  # Ограничим вывод первыми 5 объектами
                    print("...")
                    break
    except Exception as e:
        print(f"Ошибка при чтении {file_path}: {e}")

def main():
    parser = argparse.ArgumentParser(description="Inspect .msgpack file contents.")
    parser.add_argument('-i', '--input', type=str, required=True, help='Path to .msgpack file')
    args = parser.parse_args()

    inspect_msgpack(args.input)

if __name__ == "__main__":
    main()