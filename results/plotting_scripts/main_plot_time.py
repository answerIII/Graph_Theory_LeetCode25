import pandas as pd
import glob
import os
import numpy as np
import matplotlib.pyplot as plt

def plot_faceted_steps_time(csv_folder="results/landmarks_selection_and_k"):
    # 1. Map file suffix → dataset name
    name_map = {"1": "CA-AstroPh", "2": "CA-GrQc", "3": "Wiki-Vote"}
    frames = []
    # 2. Load & tag version/dataset
    for fpath in glob.glob(os.path.join(csv_folder,
                                         "perfomance_and_accuracy_VS_select_landmarks_alg_and_k_*_[123].csv")):
        df = pd.read_csv(fpath)
        fname = os.path.basename(fpath)
        version = "Basic" if "_BASIC_" in fname else "LcaSC"
        idx = fname.split("_")[-1].split(".")[0]
        dataset = name_map[idx]
        frames.append(df.assign(version=version, dataset=dataset))
    all_df = pd.concat(frames, ignore_index=True)

    # 3. Define per-version step columns
    step_cols = {
        "Basic": [
            "time_select_landmarks",
            "time_compute_landmark_distances",
            "time_estimate_distance_basic",
            "time_landmarks_basic",
        ],
        "LcaSC": [
            "time_select_landmarks",
            "time_build_spt",
            "time_get_path",
            "time_distance_sc",
            "time_landmarks_sc",
        ]
    }

    datasets = ["CA-AstroPh", "CA-GrQc", "Wiki-Vote"]
    versions = ["Basic", "LcaSC"]

    fig, axes = plt.subplots(nrows=3, ncols=2, figsize=(10, 12), sharey=True)

    for i, dataset in enumerate(datasets):
        for j, version in enumerate(versions):
            ax = axes[i, j]
            subset = all_df[(all_df["dataset"] == dataset) &
                            (all_df["version"] == version)]
            cols = step_cols[version]

            # 4. Compute mean time for each step
            means = subset[cols].mean()

            # 5. Bar positions
            x = np.arange(len(cols))
            ax.bar(x, means, edgecolor="black")

            # 6. Labels & titles
            ax.set_xticks(x)
            ax.set_xticklabels([c.replace("time_", "") for c in cols], rotation=45, ha="right")
            if j == 0:
                ax.set_ylabel(f"{dataset}\n\nTime (s)", fontsize=11)
            if i == 0:
                ax.set_title(version, fontsize=14)
            if i == 2:
                ax.set_xlabel("Step", fontsize=12)
            ax.grid(axis="y", linestyle="--", alpha=0.5)

    fig.suptitle("Average Step Times by Dataset & Version", fontsize=16)
    fig.tight_layout(rect=[0, 0, 1, 0.96])
    plt.show()


if __name__ == "__main__":
    plot_faceted_steps_time()

