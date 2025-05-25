import pandas as pd
import glob
import os
import matplotlib.pyplot as plt

def plot_sc_step_times_at_k(csv_folder="results/landmarks_selection_and_k", k=100):
    pattern = os.path.join(csv_folder, "perfomance_and_accuracy_VS_select_landmarks_alg_and_k_BASIC_*.csv")
    frames = []
    for fpath in glob.glob(pattern):
        df = pd.read_csv(fpath)
        frames.append(df)
    sc_df = pd.concat(frames, ignore_index=True)
    
    sc_k = sc_df[sc_df["k"] == k]
    
    steps = [
        "time_select_landmarks",
        "time_compute_landmark_distances",
        "time_estimate_distance_basic",
        "time_landmarks_basic",
    ]
    
    means = sc_k[steps].mean()
    
    plt.figure(figsize=(6,4))
    plt.bar(steps, means, edgecolor='black')
    plt.xlabel("step")
    plt.ylabel("average time (s)")
    plt.title(f"BASIC version: avg. step times at k = {k}")
    plt.xticks(rotation=30, ha='right')
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    plot_sc_step_times_at_k()

