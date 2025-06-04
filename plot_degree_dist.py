#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
import os

# Пути к папкам
input_dir = Path("GraphsCSVTables")
output_dir = Path("GraphImages")

# Создаем папку для графиков, если её нет
os.makedirs(output_dir, exist_ok=True)

# Проверяем существование папки с CSV
if not input_dir.is_dir():
    print(f"Ошибка: папка '{input_dir}' не существует")
    sys.exit(1)

# Находим все CSV файлы
csv_files = list(input_dir.glob('*.csv'))

if not csv_files:
    print(f"В папке '{input_dir}' не найдено CSV файлов")
    sys.exit(1)

# Обрабатываем каждый файл
for csv_file in csv_files:
    try:
        df = pd.read_csv(csv_file)
        
        fig, axes = plt.subplots(1, 2, figsize=(14,5))
        
        # Линейная шкала
        axes[0].scatter(df['k'], df['prob'], s=10)
        axes[0].set_xlabel('Степень вершины (k)')
        axes[0].set_ylabel('Вероятность P(k)')
        axes[0].set_title(f'Линейная шкала\n({csv_file.stem})')
        axes[0].grid(True, ls='--', lw=.5)
        
        # log-log шкала
        axes[1].scatter(df['k'], df['prob'], s=10)
        axes[1].set_xscale('log')
        axes[1].set_yscale('log')
        axes[1].set_xlabel('Степень вершины (k)')
        axes[1].set_ylabel('Вероятность P(k)')
        axes[1].set_title(f'log-log шкала\n({csv_file.stem})')
        axes[1].grid(True, which='both', ls='--', lw=.5)
        
        plt.tight_layout()
        
        # Сохраняем в папку GraphImages
        output_file = output_dir / f"{csv_file.stem}.png"
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        print(f"График сохранен: {output_file}")
        
    except Exception as e:
        print(f"Ошибка при обработке {csv_file.name}: {str(e)}")

print("\nОбработка завершена. Все графики сохранены в", output_dir)