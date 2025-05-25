import pandas as pd
import glob
import os
import matplotlib.pyplot as plt

def plot_basic_vs_sc(csv_folder="results/landmarks_selection_and_k"):
    pattern = f"{csv_folder}/perfomance_and_accuracy_VS_select_landmarks_alg_and_k_*.csv"
    files = glob.glob(pattern)
    
    frames = []
    for fpath in files:
        df = pd.read_csv(fpath)
        basename = os.path.basename(fpath)
        version = "SC" if "_SC_" in basename else "BASIC"
        df["version"] = version
        frames.append(df)
    all_df = pd.concat(frames, ignore_index=True)
    
    mean_df = all_df.groupby(["version", "k"])["mae"].mean().reset_index()
    
    pivot = mean_df.pivot(index="k", columns="version", values="mae")
    
    plt.figure(figsize=(6,4))
    for version in ["BASIC", "SC"]:
        plt.plot(pivot.index, pivot[version], marker='o', label=version)
    plt.xlabel("k (num of landmarks)")
    plt.ylabel("average MAE")
    plt.title("SC vs BASIC: accuracy comparison")
    plt.legend(title="algorithm")
    plt.grid(False)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    plot_basic_vs_sc()

