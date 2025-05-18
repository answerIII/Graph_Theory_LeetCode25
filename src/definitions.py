from os import path

PROJECT_ROOT_DIR = path.dirname(path.dirname(path.abspath(__file__)))

DATASETS_DIRECTED_DIR = PROJECT_ROOT_DIR + "/datasets/directed/"
DATASETS_UNDIRECTED_DIR = PROJECT_ROOT_DIR + "/datasets/undirected/"
DATASETS_LARGE_DIR = PROJECT_ROOT_DIR + "/datasets/very_large_graphs/"

REF_DATASETS_DIRECTED_DIR = PROJECT_ROOT_DIR + \
    "/reformatted_datasets/directed/"
REF_DATASETS_UNDIRECTED_DIR = PROJECT_ROOT_DIR + \
    "/reformatted_datasets/undirected/"
REF_DATASETS_LARGE_DIR = PROJECT_ROOT_DIR + \
    "/reformatted_datasets/very_large_graphs/"
