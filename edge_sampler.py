import random
import sys
import os

def sample_edges(input_path, output_path, keep_percent):
    headers = []
    edges = []

    with open(input_path, 'r') as f:
        for line in f:
            if line.startswith('#') or not line.strip():
                headers.append(line)
            else:
                edges.append(line)

    total = len(edges)
    k = int(total * keep_percent / 100)
    print(f"total edges: {total}. sampling {k} edges (~{keep_percent}% of original).")

    sampled = random.sample(edges, k)

    with open(output_path, 'w') as f_out:
        for h in headers:
            f_out.write(h)
        for e in sampled:
            f_out.write(e)

    print(f"sampled graph written to {output_path}")

if __name__ == '__main__':
    input_file = input("enter path to input graph file: ").strip()
    if not os.path.isfile(input_file):
        print(f"error: file '{input_file}' does not exist.")
        sys.exit(1)

    out_file = input("enter path for output sampled file: ").strip()
    pct = input("enter percentage of edges to keep (0-100): ").strip()
    try:
        pct_val = float(pct)
        if not (0 <= pct_val <= 100):
            raise ValueError
    except ValueError:
        print("error: elease enter a valid number between 0 and 100.")
        sys.exit(1)

    sample_edges(input_file, out_file, pct_val)

