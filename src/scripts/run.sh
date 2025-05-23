#!/bin/sh

directed=''
undirected=''
large=''

while getopts 'dul' flag; do
  case "${flag}" in
    d) directed='true';;
    u) undirected='true';;
    l) large='true';;
  esac
done

datasets=()

if [ ! -z "$directed" ]; then
  datasets=(${datasets[*]} "../datasets/directed/")
fi
if [ ! -z "$undirected" ]; then
  datasets=(${datasets[*]} "../datasets/undirected/")
fi
if [ ! -z "$large" ]; then
  datasets=(${datasets[*]} "../datasets/very_large_graphs/")
fi

for directory in ${datasets[*]}; do
  for filename in $directory*; do
    if [ ! -d "$filename" ]; then
      go run ../cmd/main.go $filename "../output/$(basename ${filename%.*})-info.txt"
    fi
  done
done
