#!/bin/sh

# Graph Generator Script
# Automatically processes all graph files from subdirectories of ../datasets/
# using the generate.go program.
#
# Usage: ./script.sh
# (No arguments needed - processes all available datasets recursively)

# Main processing loop
for directory in ../datasets/*; do       # Loop through each subdirectory in ../datasets/
  for filename in $directory/*; do       # Loop through each file in the current subdirectory
    if [ ! -d "$filename" ]; then        # Skip if this is a subdirectory (only process files)
      # Execute the graph generator program with:
      # 1. Input graph file as the only parameter
      go run generate.go $filename
    fi
  done
done