from os import path

PROJECT_ROOT_DIR = path.dirname(path.dirname(path.abspath(__file__)))

INF = 10**9

DIRECTED_FILE_NAMES = [
    "Wiki-Vote.txt",
    "soc-wiki-Vote.mtx",
    "web-Google.txt",
    "web-NotreDame.txt",
    "web-Stanford.txt",
]

UNDIRECTED_FILE_NAMES = [
    "CA-AstroPh.txt",
    "CA-GrQc.txt",
    "Email-EuAll.txt",
    "ca-coauthors-dblp.txt",
    "musae_git_edges.csv"
]

LARGE_UNDIRECTED_FILE_NAMES = [
    "com-youtube.ungraph.txt",
    "vk.csv",
    "com-orkut.ungraph.txt"
]

DATASETS_DIRECTED_DIR = PROJECT_ROOT_DIR + "/datasets/directed/"
DATASETS_UNDIRECTED_DIR = PROJECT_ROOT_DIR + "/datasets/undirected/"
DATASETS_LARGE_DIR = PROJECT_ROOT_DIR + "/datasets/very_large_graphs/"

REF_DATASETS_DIRECTED_DIR = PROJECT_ROOT_DIR + \
    "/reformatted_datasets/directed/"
REF_DATASETS_UNDIRECTED_DIR = PROJECT_ROOT_DIR + \
    "/reformatted_datasets/undirected/"
REF_DATASETS_LARGE_DIR = PROJECT_ROOT_DIR + \
    "/reformatted_datasets/very_large_graphs/"
