# Team project solution
## Reformat datasets

Create directories:\
reformatted_datasets/\
reformatted_datasets/directed/\
reformatted_datasets/undirected/\
reformatted_datasets/very_large_graphs/

Change txt files accoring to the following format:\
Nodes, Edges\
node_id node_id\
...

Run `python3 src/reformat_datasets.py`

## Largest weak component
PYTHONPATH=src pypy3 src/network_structure_analysis/a/2.py
