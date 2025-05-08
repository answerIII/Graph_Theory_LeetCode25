#include "../libs.h"
void validateArgs(int argc, const char* argv[]) {
    if (argc != 3) {
        throw std::invalid_argument("Usage: <program> <file.txt|file.csv> <true|false>");
    }

    const std::string filename = argv[1];
    std::string directedStr = argv[2];

    if (!(filename.ends_with(".txt") || filename.ends_with(".csv"))) {
        throw std::invalid_argument("Error: File must have .txt or .csv extension.");
    }

    if (directedStr != "directed" && directedStr != "undirected") {
        throw std::invalid_argument("Error: Directed flag must be 'directed' or 'undirected'.");
    }
}
