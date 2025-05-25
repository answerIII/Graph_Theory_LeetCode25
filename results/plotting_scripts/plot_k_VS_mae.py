import pandas as pd
import glob
import matplotlib.pyplot as plt

files = glob.glob("results/landmarks_selection_and_k/perfomance_and_accuracy_VS_select_landmarks_alg_and_k_*.csv")
df = pd.concat([pd.read_csv(f) for f in files], ignore_index=True)

mean_mae = df.groupby("k")["mae"].mean().reset_index()

plt.figure(figsize=(6,4))
plt.plot(mean_mae["k"], mean_mae["mae"], marker='o')
plt.xlabel("k (num of landmarks)")
plt.ylabel("average MAE")
plt.title("k VS MAE")
plt.grid(False)
plt.tight_layout()
plt.show()
