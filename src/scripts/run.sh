#!/bin/bash

# Graph processing script
# Automatically processes graph files from specified directories using a Go program
# Usage: ./script.sh [-d] [-u] [-l]
# Flags:
#   -d  Process directed graphs
#   -u  Process undirected graphs
#   -l  Process very large graphs

# Initialize flags
directed=''      # Flag for directed graphs (-d)
undirected=''    # Flag for undirected graphs (-u)
large=''         # Flag for very large graphs (-l)

# Parse command line arguments
while getopts 'dul' flag; do
  case "${flag}" in
    d) directed='true';;  # Enable directed graphs processing
    u) undirected='true';; # Enable undirected graphs processing
    l) large='true';;     # Enable very large graphs processing
  esac
done

# Initialize dataset directories array
datasets=()

# Add directories to process based on flags
if [ -n "$directed" ]; then
  datasets=(${datasets[*]} "../datasets/directed/")  # Add directed graphs directory
fi
if [ -n "$undirected" ]; then
  datasets=(${datasets[*]} "../datasets/undirected/") # Add undirected graphs directory
fi
if [ -n "$large" ]; then
  datasets=(${datasets[*]} "../datasets/very_large_graphs/") # Add large graphs directory
fi

# Main processing loop
for directory in ${datasets[*]}; do       # For each selected directory
  for filename in $directory*; do         # For each file in directory
    if [ ! -d "$filename" ]; then         # Skip subdirectories
      # Execute Go program with:
      # 1. Input graph file
      # 2. Output file (in output/ directory with original name + -info.txt suffix)
      go run ../cmd/main.go $filename "../output/$(basename ${filename%.*})-info.txt"
    fi
  done
done