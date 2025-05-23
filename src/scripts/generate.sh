#!/bin/sh
for directory in ../datasets/*; do
  for filename in $directory/*; do
    if [ ! -d "$filename" ]; then
      go run generate.go $filename
    fi
  done
done
