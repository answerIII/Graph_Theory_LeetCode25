import os
import numpy as np
import matplotlib.pyplot as plt

os.makedirs('data', exist_ok=True)
os.makedirs('graphics', exist_ok=True)

data_files = [f for f in os.listdir('data') if f.endswith('.txt')]

for filename in data_files:
    degrees = []
    frequencies = []
    with open(f'data/{filename}', 'r') as f:
        for line in f:
            if line.startswith('#') or not line.strip():
                continue
            deg, freq = map(int, line.split())
            degrees.append(deg)
            frequencies.append(freq)

    total = sum(frequencies)
    probabilities = [freq / total for freq in frequencies]

    sorted_indices = np.argsort(degrees)
    degrees_sorted = np.array(degrees)[sorted_indices]
    prob_sorted = np.array(probabilities)[sorted_indices]

    plt.figure(figsize=(14, 6))

    plt.subplot(1, 2, 1)
    plt.scatter(degrees_sorted, prob_sorted, color='blue', alpha=0.7)
    plt.xlabel('Степень вершины (k)')
    plt.ylabel('Вероятность P(k)')
    plt.title(f'Линейная шкала')
    plt.grid(True)

    plt.subplot(1, 2, 2)
    plt.loglog(degrees_sorted, prob_sorted, 'bo', alpha=0.7)
    plt.xlabel('Степень вершины (k)')
    plt.ylabel('Вероятность P(k)')
    plt.title(f'log-log шкала')
    plt.grid(True, which="both", ls="--")

    plt.tight_layout()

    output_name = os.path.splitext(filename)[0] + '.png'
    plt.savefig(f'graphics/{output_name}', dpi=300, bbox_inches='tight')
    plt.close()