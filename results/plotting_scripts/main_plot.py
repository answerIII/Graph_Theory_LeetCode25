import pandas as pd
import glob
import os
import numpy as np
import matplotlib.pyplot as plt

def plot_faceted_mae(csv_folder="results/landmarks_selection_and_k"):
    name_map = {
        "1": "CA-AstroPh",
        "2": "CA-GrQc",
        "3": "Wiki-Vote"
    }
    frames = []
    for fpath in glob.glob(os.path.join(csv_folder,
                                         "perfomance_and_accuracy_VS_select_landmarks_alg_and_k_*_[123].csv")):
        df = pd.read_csv(fpath)
        fname = os.path.basename(fpath)
        version = "BASIC" if "_BASIC_" in fname else "SC"
        file_idx = fname.split("_")[-1].split(".")[0]
        dataset = name_map[file_idx]
        frames.append(df.assign(version=version, dataset=dataset))
    all_df = pd.concat(frames, ignore_index=True)

    grouped = all_df.groupby(["dataset","version","method","k"])["mae"].mean().reset_index()

    datasets = ["CA-AstroPh", "CA-GrQc", "Wiki-Vote"]
    versions = ["BASIC", "SC"]
    methods = ["random", "degree", "coverage"]
    ks = sorted(all_df["k"].unique())

    fig, axes = plt.subplots(nrows=3, ncols=2, figsize=(10, 12), sharex=True, sharey=True)
    bar_width = 0.25
    x = np.arange(len(ks))

    for i, dataset in enumerate(datasets):
        for j, version in enumerate(versions):
            ax = axes[i, j]
            sub = grouped[(grouped["dataset"]==dataset) & (grouped["version"]==version)]
            pivot = sub.pivot(index="k", columns="method", values="mae").reindex(ks)
            for m, method in enumerate(methods):
                if method in pivot.columns:
                    ax.bar(x + (m-1)*bar_width,
                           pivot[method],
                           width=bar_width,
                           label=method.capitalize() if (i==0 and j==0) else "",
                           alpha=0.8)
            if i==0:
                ax.set_title(version, fontsize=14)
            if j==0:
                ax.set_ylabel(f"{dataset}\n\nMAE", fontsize=12)
            if i==2:
                ax.set_xticks(x)
                ax.set_xticklabels(ks)
                ax.set_xlabel("k (num of landmarks)", fontsize=12)
            ax.grid(axis="y", linestyle="--", alpha=0.5)

    handles, labels = axes[0,0].get_legend_handles_labels()
    fig.legend(handles, labels, title="landmark selection method",
               loc="upper right", bbox_to_anchor=(0.92, 0.92))
    fig.suptitle("MAE, version, k & selection method", fontsize=16)
    fig.tight_layout(rect=[0,0,0.9,0.96])
    plt.show()

if __name__ == "__main__":
    plot_faceted_mae()
