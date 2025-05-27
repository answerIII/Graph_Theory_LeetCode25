#ifndef PARSER_H
#define PARSER_H
#include "../libs.h"
#include "Graph.h"
class Parser {
public:
    virtual ~Parser() = default;
    virtual Graph* parse(const char* file, bool directed) = 0;
};
class TxtParser : public Parser {
public:
    Graph* parse(const char* file, bool directed) override {
        std::ifstream inFile(file);
        if (!inFile) {
            throw std::runtime_error("Failed to open file");
        }
        std::vector<std::vector<int>> paths;
        std::string line;
        int vertexCount = 0;
        int edgeCount = 0;
        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '#') {
                if (line[2] == 'N' && line[3] == 'o') {
                    std::istringstream iss(line);
                    std::string token;
                    while (iss >> token) {
                        if (token == "Nodes:") iss >> vertexCount;
                        else if (token == "Edges:") iss >> edgeCount;
                    }
                }
            } else {
                break;
            }
        }
        if (vertexCount == 0) {
            throw std::runtime_error("Could not read vertex count from file");
        }
        std::vector<Node> vertexes;
        paths.resize(vertexCount);
        vertexes.resize(vertexCount);

        std::istringstream iss(line);
        int from, to;
        iss >> from >> to;
        if (vertexes[from].num == -1){vertexes[from] = Node(from,false);}
        if (vertexes[to].num == -1){vertexes[to] = Node(to,false);}
        paths[from].push_back(to);
        if (!directed){paths[to].push_back(from);}

        while (inFile >> from >> to) {
            if (vertexes[from].num == -1){vertexes[from] = Node(from,false);}
            if (vertexes[to].num == -1){vertexes[to] = Node(to,false);}
            paths[from].push_back(to);
            if (!directed){paths[to].push_back(from);}
        }
        return new Graph(vertexes, paths, vertexCount,edgeCount);
    }
};
#endif //PARSER_H