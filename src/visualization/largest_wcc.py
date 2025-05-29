import re
import os
import matplotlib.pyplot as plt

script_dir = os.path.dirname(__file__)
data_dir = r'C:\Users\Dmitry\Desktop\Graph_Theory_LeetCode25\src\output'
graphics_dir = os.path.join(script_dir, 'largest_wcc')

os.makedirs(graphics_dir, exist_ok=True)
data_files = [f for f in os.listdir(data_dir)]

for filename in data_files:
    file_path = os.path.join(data_dir, filename)
    with open(file_path, 'r', encoding='utf-8') as f:
        random = []
        biggest = []
        for s in f:
            if 'Удаление случайных узлов:' in s or 'Удаление узлов наибольшей степени:' in s:
                start = s
                start = start.replace('\t', '').replace('\n', '')

            if 'x = ' in s and start == 'Удаление случайных узлов:':
                s = s.replace('\t\t', '').replace('\n', '')
                random.append(s)
            elif 'x = ' in s and start == 'Удаление узлов наибольшей степени:':
                s = s.replace('\t\t', '').replace('\n', '')
                biggest.append(s)

        random_data = {}
        for line in random:
            x = float(re.search(r'x\s*=\s*(\d+)%', line).group(1))
            wcc = float(re.search(r':\s*([\d.]+)', line).group(1))
            random_data[x] = wcc

        biggest_data = {}
        for line in biggest:
            x = float(re.search(r'x\s*=\s*([\d.]+)%', line).group(1))
            wcc = float(re.search(r':\s*([\d.]+)', line).group(1))
            biggest_data[x] = wcc

    x_random = list(random_data.keys())
    y_random = list(random_data.values())

    x_degree = list(biggest_data.keys())
    y_degree = list(biggest_data.values())

    plt.figure(figsize=(12, 7))
    plt.plot(x_random, y_random, 'b-o', label='Случайное удаление', linewidth=2, markersize=8)
    plt.plot(x_degree, y_degree, 'r--s', label='Удаление по степени', linewidth=2, markersize=6)

    plt.xlabel('Процент удалённых узлов (%)', fontsize=12)
    plt.ylabel('Доля вершин в наибольшей WCC', fontsize=12)
    plt.legend(fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)

    if 0.01 in x_degree:
        plt.xscale('log')
        plt.xticks([0.01, 0.1, 1, 10, 100], ['0.01%', '0.1%', '1%', '10%', '100%'])
        plt.ylim(-0.02, 1.02)

    plt.tight_layout()

    output_name = os.path.splitext(filename)[0] + '.png'
    output_path = os.path.join(graphics_dir, output_name)
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()