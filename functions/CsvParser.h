#ifndef CSVPARSER_H
#define CSVPARSER_H

#include "../libs.h"
#include "../classes/Graph.h"

inline std::string get_file_name(const std::string& filename) {
    size_t dot_pos = filename.find_last_of('.');
    std::string name_without_ext;
    if (dot_pos != std::string::npos) {
        name_without_ext = filename.substr(0, dot_pos);
    }
    return name_without_ext;
}

inline void general_properties(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    const std::string filename = "01_general_properties_"+get_file_name(graph_id)+".csv";
    std::ofstream file("output_data/01_general_properties/"+filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,num_vertices,num_edges,density,"
         << "weak_comp_count,weak_largest_comp_size,weak_largest_comp_frac,"
         << "strong_comp_count,strong_largest_comp_frac\n";

    // Data rows

        file << graph_id << ","
             << is_directed << ","
             << g.getVertexCount() << ","
             << g.getEdgesCount() << ","
             << g.getDensity() << ","
             << g.getWeekComponentCount() << ","
             << g.getShareVertexInBeggestWeekComponent()<< ","
             << g.getShareVertexInBeggestWeekComponent() << ","
             << g.getStrongestComponentCount() << ","
             << g.getShareVertexInBeggestStrongComponent() << "\n";


    file.close();
    std::cout << "Graph info for " + graph_id +  " is written to " << filename << "\n";
}

inline void double_sweep(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    const std::string filename = "02a_double_sweep"+get_file_name(graph_id)+".csv";
    std::ofstream file("output_data/02a_double_sweep/"+filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,seed_node,farthest_node,diameter_est\n";

    // Data rows

    file << graph_id << ","
         << is_directed << ",";


    file.close();
    std::cout << "Graph info for " + graph_id +  " is written to " << filename << "\n";
}

inline void random_sample(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    const std::string filename = "02b_random_sample"+get_file_name(graph_id)+".csv";
    std::ofstream file("output_data/02b_random_sample/"+filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,sample_size,seed,num_pairs,"
         << "mean_dist,median_dist,p90_dist,max_dist\n";

    // Data rows

    file << graph_id << ","
         << is_directed << ",";


    file.close();
    std::cout << "Graph info for " + graph_id +  " is written to " << filename << "\n";
}

inline void snowball(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    const std::string filename = "02c_snowball"+get_file_name(graph_id)+".csv";
    std::ofstream file("output_data/02c_snowball/"+filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,target_size,seed,num_pairs,"
         << "mean_dist,median_dist,p90_dist, max_dist\n";

    // Data rows

    file << graph_id << ","
         << is_directed << ",";


    file.close();
    std::cout << "Graph info for " + graph_id +  " is written to " << filename << "\n";
}

inline void clustering_overview(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    const std::string filename = "03_clustering_overview"+get_file_name(graph_id)+".csv";
    std::ofstream file("output_data/03_clustering_overview/"+filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,num_triangles,global_clustering_coef,"
         << "avg_clustering_coef\n";

    // Data rows

    file << graph_id << ","
         << is_directed << ",";


    file.close();
    std::cout << "Graph info for " + graph_id +  " is written to " << filename << "\n";
}

inline void local_clustering(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    const std::string filename = "04_local_clustering"+get_file_name(graph_id)+".csv";
    std::ofstream file("output_data/04_local_clustering/"+filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,node_id,degree,num_edges_between_neigh,"
         << "local_clustering_coef\n";

    // Data rows

    file << graph_id << ","
         << is_directed << ",";


    file.close();
    std::cout << "Graph info for " + graph_id +  " is written to " << filename << "\n";
}

inline void degree_distribution(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    const std::string filename = "05_degree_distribution"+get_file_name(graph_id)+".csv";
    std::ofstream file("output_data/05_degree_distribution/"+filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,degree_k,count_nodes,probability,"
         << "log_k,log_prob\n";

    g.getVertexDegree(graph_id, is_directed, file);

    file.close();
    std::cout << "Graph info for " + graph_id +  " is written to " << filename << "\n";
}
inline void robustness(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    const std::string filename = "06_robustness"+get_file_name(graph_id)+".csv";
    std::ofstream file("output_data/06_robustness/"+filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,removal_method,x_percent,run_id,"
         << "remaining_nodes,largest_comp_frac\n";

    // Data rows

    file << graph_id << ","
         << is_directed << ",";


    file.close();
    std::cout << "Graph info for " + graph_id +  " is written to " << filename << "\n";
}

inline void algorithms_runtime(const std::string& graph_id, const std::string& is_directed, DirectedGraph& g) {
    const std::string filename = "07_algorithms_runtime"+get_file_name(graph_id)+".csv";
    std::ofstream file("output_data/07_algorithms_runtime/"+filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    file << "graph_id,directed,algorithm,params,run_id,"
         << "start_timestamp,elapsed_ms,memory_used_mb\n";

    // Data rows

    file << graph_id << ","
         << is_directed << ",";


    file.close();
    std::cout << "Graph info for " + graph_id +  " is written to " << filename << "\n";
}
#endif //CSVPARSER_H
