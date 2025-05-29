#!/usr/bin/env python3
import sys, pandas as pd, matplotlib.pyplot as plt
from pathlib import Path

csv = Path(sys.argv[1])
df = pd.read_csv(csv)

fig, axes = plt.subplots(1, 2, figsize=(14,5))

# линейная
axes[0].scatter(df['k'], df['prob'], s=10)
axes[0].set_xlabel('Степень вершины (k)')
axes[0].set_ylabel('Вероятность P(k)')
axes[0].set_title('Линейная шкала')
axes[0].grid(True, ls='--', lw=.5)

# log-log
axes[1].scatter(df['k'], df['prob'], s=10)
axes[1].set_xscale('log')
axes[1].set_yscale('log')
axes[1].set_xlabel('Степень вершины (k)')
axes[1].set_ylabel('Вероятность P(k)')
axes[1].set_title('log-log шкала')
axes[1].grid(True, which='both', ls='--', lw=.5)

plt.tight_layout()
plt.show()
