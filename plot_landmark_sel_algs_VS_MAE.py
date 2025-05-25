import pandas as pd
import glob
import matplotlib.pyplot as plt

def plot_method_comparison(csv_folder="results/landmarks_selection_and_k"):
    pattern = f"{csv_folder}/perfomance_and_accuracy_VS_select_landmarks_alg_and_k_*.csv"
    files = glob.glob(pattern)
    df = pd.concat((pd.read_csv(f) for f in files), ignore_index=True)

    mean_df = (
        df
        .groupby(["method", "k"])["mae"]
        .mean()
        .reset_index()
    )

    pivot_df = mean_df.pivot(index="k", columns="method", values="mae")

    plt.figure(figsize=(6,4))
    for method in ["random", "degree", "coverage"]:
        if method in pivot_df:
            plt.plot(pivot_df.index, pivot_df[method], marker='o', label=method.capitalize())

    plt.xlabel("k (num of landmarks)")
    plt.ylabel("average MAE")
    plt.title("comparison of landmark-selection methods")
    plt.legend(title="method")
    plt.grid(False)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    plot_method_comparison()

