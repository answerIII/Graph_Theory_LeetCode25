#ifndef CSVPARSER_H
#define CSVPARSER_H

#include "../libs.h"
#include "../classes/Graph.h"

bool isFileEmpty(const std::string& filename) {
    struct stat stat_buf;
    if (stat(filename.c_str(), &stat_buf) != 0) {
        // File doesn't exist or can't be accessed - treat as empty
        return true;
    }
    return stat_buf.st_size == 0;
}

inline std::string get_file_name(const std::string& path) {
    size_t slash_pos = path.find_last_of("/\\");
    std::string filename = (slash_pos == std::string::npos) ? path : path.substr(slash_pos + 1);

    size_t dot_pos = filename.find_last_of('.');
    if (dot_pos != std::string::npos) {
        filename = filename.substr(0, dot_pos);
    }

    return filename;
}

inline void log_runtime_memory(const std::string& graph_id, const std::string& is_directed,
                               const std::string& algorithm, const std::string& params, int run_id,
                               const std::chrono::steady_clock::time_point& start_time) {
    const std::string filename = "07_algorithms_runtime_" + get_file_name(graph_id) + ".csv";

    std::filesystem::create_directories("output_data/07_algorithms_runtime");

    std::ofstream file("output_data/07_algorithms_runtime/" + filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    if (!isFileEmpty(filename)) {
        file << "graph_id, algorithm, params, run_id, start_timestamp, elapsed_ms, memory_used_mb\n";
    }
    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    size_t memoryUsed = memInfo.WorkingSetSize / (1024 * 1024); // MB

    file << graph_id << ","
         << is_directed << ","
         << algorithm << ","
         << params << ","
         << run_id << ","
         << std::chrono::duration_cast<std::chrono::milliseconds>(start_time.time_since_epoch()).count() << ","
         << elapsed << ","
         << memoryUsed << "\n";

    file.close();
}

inline void general_properties(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    std::string base_name = get_file_name(graph_id);
    const std::string filename = "01_general_properties_" + base_name + ".csv";

    std::filesystem::create_directories("output_data/01_general_properties");

    std::ofstream file("output_data/01_general_properties/" + filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }
    file << "graph_id,directed,num_vertices,num_edges,density,"
         << "weak_comp_count,weak_largest_comp_size,weak_largest_comp_frac,"
         << "strong_comp_count,strong_largest_comp_size,strong_largest_comp_frac\n";

    auto start = std::chrono::steady_clock::now();
        file << base_name << ","
             << is_directed << ","
             << g.getVertexCount() << ","
             << g.getEdgesCount() << ","
             << g.getDensity() << ","
             << g.getWeekComponentCount() << ","
             << g.getCountNodesInLargestWCC()<< ","
             << g.getShareVertexInBeggestWeekComponent() << ","
             << g.getStrongestComponentCount() << ","
             << g.getCountNodesInLargestSCC() << ","
             << g.getShareVertexInBeggestStrongComponent() << "\n";

    log_runtime_memory(base_name, is_directed, "general_properties", "", 0, start);
    file.close();
    std::cout << "Graph info for " + base_name +  " is written to " << filename << "\n";
}

inline void double_sweep(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    std::string base_name = get_file_name(graph_id);
    const std::string filename = "02a_double_sweep_"+base_name+".csv";

    std::filesystem::create_directories("output_data/02a_double_sweep");

    std::ofstream file("output_data/02a_double_sweep/"+filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,weak_largest_comp_size,diameter_est,seed_node,farthest_node\n";

    auto start = std::chrono::steady_clock::now();

    file << base_name << ","
        << is_directed << ","
        << g.getCountNodesInLargestWCC() << ","
        << g.getApproximateDiameter() << ","
        << g.getFarthestNode()->num << ","
        << g.getSeedNode()->num<< "\n";

    log_runtime_memory(base_name, is_directed, "double_sweep", "", 0, start);
    file.close();
    std::cout << "Graph info for " + base_name +  " is written to " << filename << "\n";
}

inline void random_sample(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    std::string base_name = get_file_name(graph_id);
    const std::string filename = "02b_random_sample_"+base_name+".csv";

    std::filesystem::create_directories("output_data/02b_random_sample");

    std::ofstream file("output_data/02b_random_sample/"+filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,weak_largest_comp_size,sample_size,p90_dist,seed,num_pairs,"
         << "mean_dist,median_dist,max_dist\n";

    auto start = std::chrono::steady_clock::now();
    file << base_name << ","
         << is_directed << ","
         << g.getCountNodesInLargestWCC() << ","
         << 500 << ","
         << g.get90PercentileB() << ","
         << g.getSeedUsed() << ","
         << g.getUsedPairs() << ","
         << g.getMeanDist() << ","
         << g.getMedianDist() << ","
         << g.getMaxDist() << "\n";

    log_runtime_memory(base_name, is_directed, "random_sample", "", 0, start);
    file.close();
    std::cout << "Graph info for " + base_name +  " is written to " << filename << "\n";

}

inline void snowball(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    std::string base_name =get_file_name(graph_id);

    const std::string filename = "02c_snowball_"+base_name+".csv";

    std::filesystem::create_directories("output_data/02c_snowball");

    std::ofstream file("output_data/02c_snowball/"+filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,p90_dist,target_size,seed,num_pairs,"
         << "mean_dist,median_dist,max_dist\n";

    auto start = std::chrono::steady_clock::now();

    file << base_name << ","
         << is_directed << ","
         << g.get90PercentileC() << ","
         << g.getSnowballSampleSize() << ","
         << g.getSnowballSeed() << ","
         << g.getSnowballSamplePairs() << ","
         << g.getSnowballMean() << ","
         << g.getSnowballMedian() << ","
         << g.getSnowballP90() << ","
         << g.getSnowballMax() << "\n";


    log_runtime_memory(base_name, is_directed, "snowball", "", 0, start);

    file.close();
    std::cout << "Graph info for " + base_name +  " is written to " << filename << "\n";

}

inline void clustering_overview(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    std::string base_name = get_file_name(graph_id);
    const std::string filename = "03_clustering_overview_"+base_name+".csv";

    std::filesystem::create_directories("output_data/03_clustering_overview");

    std::ofstream file("output_data/03_clustering_overview/"+filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,num_triangles,global_clustering_coef,"
         << "avg_clustering_coef\n";

    auto start = std::chrono::steady_clock::now();

    file << base_name << ","
         << is_directed << ","
         << g.getTrianglesCount()<< ","
         << g.getAverageClusteringCoefficient() << ","
         << g.getGlobalClusteringCoefficient() << ",";
    log_runtime_memory(base_name, is_directed, "clustering_overview", "", 0, start);

    std::cout << "Graph info for " + base_name +  " is written to " << filename << "\n";
    file.close();

}

inline void local_clustering(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    std::string base_name = get_file_name(graph_id);
    const std::string filename = "04_local_clustering_"+base_name+".csv";

    std::filesystem::create_directories("output_data/04_local_clustering");

    std::ofstream file("output_data/04_local_clustering/"+filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,node_id,degree,num_edges_between_neigh,"
         << "local_clustering_coef\n";

    auto start = std::chrono::steady_clock::now();
    g.getAverageClusteringCoefficientOfWCC(base_name, is_directed, file);

    log_runtime_memory(base_name, is_directed, "degree_distribution", "", 0, start);

    file.close();
    std::cout << "Graph info for " + base_name +  " is written to " << filename << "\n";

}

inline void degree_distribution(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    std::string base_name = get_file_name(graph_id);
    const std::string filename = "05_degree_distribution_"+base_name+".csv";

    std::filesystem::create_directories("output_data/05_degree_distribution");

    std::ofstream file("output_data/05_degree_distribution/"+filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,deg_type,degree_k,count_nodes,probability,"
         << "log_k,log_prob\n";
    auto start = std::chrono::steady_clock::now();

    g.getVertexDegree(base_name, is_directed, file);
    log_runtime_memory(base_name, is_directed, "degree_distribution", "", 0, start);

    file.close();
    std::cout << "Graph info for " + base_name +  " is written to " << filename << "\n";

}

inline void robustness(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    std::string base_name = get_file_name(graph_id);
    const std::string filename = "06_robustness_"+base_name+".csv";

    std::filesystem::create_directories("output_data/06_robustness");

    std::ofstream file("output_data/06_robustness/"+filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,removal_method,x_percent,run_id,"
         << "remaining_nodes,largest_comp_frac\n";

    // Data rows
    auto start = std::chrono::steady_clock::now();

    file << graph_id << ","
         << is_directed << ",";
         //<< g.removeRandomNodes() << ","
         //<< g.removeMostDegreesNodes() << ",";

    log_runtime_memory(base_name, is_directed, "degree_distribution", "", 0, start);

    file.close();
    std::cout << "Graph info for " + base_name +  " is written to " << filename << "\n";
}

#endif //CSVPARSER_H
