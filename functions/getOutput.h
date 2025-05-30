#ifndef GETOUTPUT_H
#define GETOUTPUT_H
#include "../libs.h"
#include "../classes/Graph.h"
#include "../functions/CsvParser.h"

std::string line(100, '=');
namespace fs = std::filesystem;

struct GraphFile {
    std::string display_name;
    std::string path;
    std::string category;
};

void get_results(const char* file_path, const std::string& category, int algo) {
    bool directed = (category == "directed");
    const std::string& is_directed = "directed";

    TxtParser parser;
    Graph* graph = parser.parse(file_path, directed);

    DirectedGraph g(*graph);
    if (algo == 1) {
        general_properties(file_path, is_directed,g);
        double_sweep(file_path, is_directed,g);
        random_sample(file_path, is_directed,g);
        snowball(file_path, is_directed,g);
        clustering_overview(file_path, is_directed,g);
        local_clustering(file_path, is_directed,g);
        degree_distribution(file_path, is_directed,g);
        robustness(file_path, is_directed,g);
    }else if (algo == 2) {
        int v1 = -1;
        int v2 = -1;
        std::cout << "Количество вершин в графе:\t"<< g.getVertexCount() <<"\n";
        std::cout <<"Выберите вершины:\n";
        std::cout << "v1:";
        std::cin >> v1;
        std::cout << "v2:";
        std::cin >> v2;
        int result = g.getDistanceBetweenNodes(v1,v2);
        std::cout << line;
        std::cout <<"Расстояние между вершинами " <<v1 << " и " << v2 << " равно:\t" <<result<<"\n";
        std::cout << line;
    }

    delete graph;
}

void merge_csv_files_in_folder(const std::string& folder_path) {
    for (const auto& subfolder : fs::directory_iterator(folder_path)) {
        if (!subfolder.is_directory()) continue;

        std::string merged_filename = subfolder.path().filename().string() + "_merged.csv";
        std::string output_path = folder_path + "/" + merged_filename;

        std::ofstream output(output_path);
        bool header_written = false;

        for (const auto& file : fs::directory_iterator(subfolder.path())) {
            if (!file.is_regular_file()) continue;

            std::ifstream input(file.path());
            std::string line;
            std::string filename = file.path().filename().string();
            std::string graph_name = filename.substr(0, filename.find(".csv"));

            bool is_header = true;
            while (std::getline(input, line)) {
                if (is_header) {
                    if (!header_written) {
                        output << "graph_name," << line << "\n";
                        header_written = true;
                    }
                    is_header = false;
                } else {
                    output << graph_name << "," << line << "\n";
                }
            }
        }

        std::cout << "Объединён: " << output_path << "\n";
    }
}

#endif //GETOUTPUT_H
