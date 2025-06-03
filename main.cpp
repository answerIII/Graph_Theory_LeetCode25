#include "Graph/Graph.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <random>

namespace fs = std::filesystem;
using Clock = std::chrono::high_resolution_clock;

std::vector<std::string> collectGraphFiles(const std::string& folder) {
    std::vector<std::string> files;
    for (const auto& entry : fs::recursive_directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

std::string detectFormat(const std::string& path) {
    auto ext = fs::path(path).extension().string();
    if (ext == ".csv") return "csv";
    if (ext == ".mtx") return "mtx";
    return "txt";
}

void ensureResultsFolder() {
    fs::create_directories("results/graphs");
    fs::create_directories("results/tasks");
    for (const auto& entry : fs::directory_iterator("results/graphs")) {
        if (entry.path().extension() == ".csv") {
            fs::remove(entry);
        }
    }
    for (const auto& entry : fs::directory_iterator("results/tasks")) {
        if (entry.path().extension() == ".csv") {
            fs::remove(entry);
        }
    }
}

std::unordered_set<std::string> writtenHeaders;

std::string formatDuration(std::chrono::duration<double> dur) {
    double seconds = dur.count();
    if (seconds < 1.0) {
        return std::to_string(static_cast<int>(seconds * 1000)) + " ms";
    } else {
        return std::to_string(seconds) + " s";
    }
}

void appendGraphMetric(const std::string& graphPath, const std::string& metric, const std::string& value) {
    std::string graphName = fs::path(graphPath).filename().string();
    std::ofstream outGraph("results/graphs/" + graphName + ".csv", std::ios::app);
    outGraph << metric << "," << value << "\n";
    outGraph.close();
}

void appendTaskMetric(const std::string& task, const std::string& graphPath, const std::vector<std::string>& values, const std::vector<std::string>& headers) {
    std::ofstream out("results/tasks/" + task + ".csv", std::ios::app);
    static std::unordered_set<std::string> initialized;
    if (!initialized.count(task)) {
        for (size_t i = 0; i < headers.size(); ++i) {
            out << headers[i];
            if (i + 1 < headers.size()) out << ",";
        }
        out << std::endl;
        initialized.insert(task);
    }
    for (size_t i = 0; i < values.size(); ++i) {
        out << values[i];
        if (i + 1 < values.size()) out << ",";
    }
    out << std::endl;
    out.close();
}

int main() {
    ensureResultsFolder();
    auto graphFiles = collectGraphFiles("datasets");

    int current = 0;
    for (const auto& path : graphFiles) {
        std::cout << "Загружается граф: " << path << std::endl;
        Graph g;
        auto loadStart = Clock::now();
        try {
            g.loadFromFile(path, detectFormat(path));
        } catch (...) {
            continue;
        }
        auto loadEnd = Clock::now();
        std::string loadTime = formatDuration(loadEnd - loadStart);
        appendGraphMetric(path, "время загрузки графа", loadTime);

        // === Задание 1A1 ===
        auto t_vc = Clock::now(); int vc = g.getVertexCount(); std::string tvc = formatDuration(Clock::now() - t_vc);
        appendGraphMetric(path, "количество вершин", std::to_string(vc));
        appendGraphMetric(path, "время подсчета количества вершин", tvc);

        auto t_ec = Clock::now(); int ec = g.getEdgeCount(); std::string tec = formatDuration(Clock::now() - t_ec);
        appendGraphMetric(path, "количество ребер", std::to_string(ec));
        appendGraphMetric(path, "время подсчета количества ребер", tec);

        auto t_dens = Clock::now(); double dens = g.getDensity(); std::string tdens = formatDuration(Clock::now() - t_dens);
        appendGraphMetric(path, "плотность графа", std::to_string(dens));
        appendGraphMetric(path, "время подсчета плотности", tdens);

        auto t_wcc = Clock::now(); int wcc = g.countWeaklyConnectedComponents(); std::string twcc = formatDuration(Clock::now() - t_wcc);
        appendGraphMetric(path, "кол-во слабосвязных компонент", std::to_string(wcc));
        appendGraphMetric(path, "время подсчета WCC", twcc);

        auto t_scc = Clock::now(); int scc = g.countStronglyConnectedComponents(); std::string tscc = formatDuration(Clock::now() - t_scc);
        appendGraphMetric(path, "кол-во сильно связанных компонент", std::to_string(scc));
        appendGraphMetric(path, "время подсчета SCC", tscc);

        auto t_wccr = Clock::now(); double wccr = g.getWCCRatio(); std::string twccr = formatDuration(Clock::now() - t_wccr);
        appendGraphMetric(path, "доля вершин в самой большой WCC", std::to_string(wccr));
        appendGraphMetric(path, "время подсчета доли WCC", twccr);

        auto t_sccr = Clock::now(); double sccr = g.getLargestSCCRatio(); std::string tsccr = formatDuration(Clock::now() - t_sccr);
        appendGraphMetric(path, "доля вершин в самой большой SCC", std::to_string(sccr));
        appendGraphMetric(path, "время подсчета доли SCC", tsccr);
        
        std::vector<std::string> vals1A1 = {path, loadTime, std::to_string(vc), tvc, std::to_string(ec), tec, std::to_string(dens), tdens, std::to_string(wcc), twcc, std::to_string(scc), tscc, std::to_string(wccr), twccr, std::to_string(sccr), tsccr};
        std::vector<std::string> heads1A1 = {"file", "load time", "vertex count", "vertex time", "edge count", "edge time", "density", "density time", "WCC count", "WCC time", "SCC count", "SCC time", "WCC ratio", "WCC ratio time", "SCC ratio", "SCC ratio time"};
        appendTaskMetric("1A1", path, vals1A1, heads1A1);
        std::cout << "[1A1]" << std::endl;      

        // === Задание 1A2 ===
        auto t_d1 = Clock::now(); int d1 = g.estimateDiameterDoubleSweep(); std::string td1 = formatDuration(Clock::now() - t_d1);
        appendGraphMetric(path, "оценка диаметра double sweep", std::to_string(d1));
        appendGraphMetric(path, "время оценки double sweep", td1);

        auto t_dist = Clock::now(); auto [avgDist, maxDist] = g.distanceStatsRandomPairs(); std::string tdist = formatDuration(Clock::now() - t_dist);
        appendGraphMetric(path, "среднее расстояние (рандомные пары)", std::to_string(avgDist));
        appendGraphMetric(path, "максимальное расстояние (рандомные пары)", std::to_string(maxDist));
        appendGraphMetric(path, "время подсчета дистанций (рандом)", tdist);

        auto t_snow = Clock::now(); auto [d2, p90] = g.snowballDiameterAndP90(); std::string tsnow = formatDuration(Clock::now() - t_snow);
        appendGraphMetric(path, "оценка диаметра snowball", std::to_string(d2));
        appendGraphMetric(path, "P90 расстояний snowball", std::to_string(p90));
        appendGraphMetric(path, "время snowball оценки", tsnow);

        std::vector<std::string> vals1A2 = {path, loadTime, std::to_string(d1), td1, std::to_string(avgDist), tdist, std::to_string(maxDist), tdist, std::to_string(d2), tsnow, std::to_string(p90), tsnow};
        std::vector<std::string> heads1A2 = {"file", "load time", "diameter1", "diameter1 time", "avgDist", "avgDist time", "maxDist", "maxDist time", "diameter2", "diameter2 time", "p90_2", "p90_2 time"};
        appendTaskMetric("1A2", path, vals1A2, heads1A2);
        std::cout << "[1A2]" << std::endl;

        // === Задание 1A3 ===
        auto t_tri = Clock::now(); int tri = g.countTriangles(); std::string ttri = formatDuration(Clock::now() - t_tri);
        appendGraphMetric(path, "число треугольников", std::to_string(tri));
        appendGraphMetric(path, "время подсчета треугольников", ttri);

        auto t_acc = Clock::now(); double acc = g.averageClusteringCoefficient(); std::string tacc = formatDuration(Clock::now() - t_acc);
        appendGraphMetric(path, "средний коэфф. кластеризации", std::to_string(acc));
        appendGraphMetric(path, "время среднего кластеризации", tacc);

        auto t_gcc = Clock::now(); double gcc = g.globalClusteringCoefficient(); std::string tgcc = formatDuration(Clock::now() - t_gcc);
        appendGraphMetric(path, "глобальный коэфф. кластеризации", std::to_string(gcc));
        appendGraphMetric(path, "время глобального кластеризации", tgcc);

        std::vector<std::string> vals1A3 = {path, loadTime, std::to_string(tri), ttri, std::to_string(acc), tacc, std::to_string(gcc), tgcc};
        std::vector<std::string> heads1A3 = {"file", "load time", "triangle count", "triangle time", "avg clustering", "avg clustering time", "global clustering", "global clustering time"};
        appendTaskMetric("1A3", path, vals1A3, heads1A3);
        std::cout << "[1A3]" << std::endl;
      

        // === Задание 1A4 ===
        auto t_awcc = Clock::now(); double awcc = g.averageClusteringLargestWCC(); std::string tawcc = formatDuration(Clock::now() - t_awcc);
        appendGraphMetric(path, "средний коэфф. кластеризации в WCC", std::to_string(awcc));
        appendGraphMetric(path, "время кластеризации в WCC", tawcc);

        std::vector<std::string> vals1A4 = {path, loadTime, std::to_string(awcc), tawcc};
        std::vector<std::string> heads1A4 = {"file", "load time", "avg clustering in WCC", "time"};
        appendTaskMetric("1A4", path, vals1A4, heads1A4);
        std::cout << "[1A4]" << std::endl;
       
        // === Задание 1A5 ===
        auto t_deg = Clock::now(); auto [mindeg, maxdeg, avgdeg] = g.getDegreeStats(); std::string tdeg = formatDuration(Clock::now() - t_deg);
        appendGraphMetric(path, "мин. степень", std::to_string(mindeg));
        appendGraphMetric(path, "макс. степень", std::to_string(maxdeg));
        appendGraphMetric(path, "средняя степень", std::to_string(avgdeg));
        appendGraphMetric(path, "время подсчета степеней", tdeg);

        std::vector<std::string> vals1A5 = {path, loadTime, std::to_string(mindeg), tdeg, std::to_string(maxdeg), tdeg, std::to_string(avgdeg), tdeg};
        std::vector<std::string> heads1A5 = {"file", "load time", "min deg", "min deg time", "max deg", "max deg time", "avg deg", "avg deg time"};
        appendTaskMetric("1A5", path, vals1A5, heads1A5);
        std::cout << "[1A5]" << std::endl;     

        // === Задание 1B ===
        auto t_rnd = Clock::now(); double r1 = g.ratioAfterRemoval(0.05, false); std::string tr1 = formatDuration(Clock::now() - t_rnd);
        appendGraphMetric(path, "доля вершин после случайного удаления", std::to_string(r1));
        appendGraphMetric(path, "время случайного удаления", tr1);

        auto t_tar = Clock::now(); double r2 = g.ratioAfterRemoval(0.05, true); std::string tr2 = formatDuration(Clock::now() - t_tar);
        appendGraphMetric(path, "доля вершин после целевого удаления", std::to_string(r2));
        appendGraphMetric(path, "время целевого удаления", tr2);

        std::vector<std::string> vals1B = {path, loadTime, std::to_string(r1), tr1, std::to_string(r2), tr2};
        std::vector<std::string> heads1B = {"file", "load time", "random removal ratio", "random time", "targeted removal ratio", "targeted time"};
        appendTaskMetric("1B", path, vals1B, heads1B);
        std::cout << "[1B]" << std::endl;

        // === Задание 2 ===
        auto t_lm = Clock::now(); g.precomputeLandmarks(64, Graph::LandmarkSelect::Random); std::string tlm = formatDuration(Clock::now() - t_lm);
        appendGraphMetric(path, "время препроцессинга landmarks", tlm);

        int n = g.getVertexCount();
        std::mt19937 rng(123);
        std::uniform_int_distribution<int> dist(0, n - 1);
        int numPairs = 100, sumDist = 0, validPairs = 0;

        auto t_appr = Clock::now();
        for (int j = 0; j < numPairs; ++j) {
            int s = dist(rng), t = dist(rng);
            while (s == t) t = dist(rng);
            int approx = g.landmarkBasicDistance(s, t);
            if (approx >= 0) {
                sumDist += approx;
                ++validPairs;
            }
        }
        std::string tappr = formatDuration(Clock::now() - t_appr);
        double avg = validPairs > 0 ? static_cast<double>(sumDist) / validPairs : -1.0;
        appendGraphMetric(path, "количество допустимых пар", std::to_string(validPairs));
        appendGraphMetric(path, "среднее расстояние по landmarks", std::to_string(avg));
        appendGraphMetric(path, "время подсчета по landmarks", tappr);

        std::vector<std::string> vals2 = {path, loadTime, std::to_string(validPairs), tappr, std::to_string(avg), tappr, tlm};
        std::vector<std::string> heads2 = {"file", "load time", "valid pairs", "pairs time", "avg landmark dist", "avg dist time", "landmark preprocess time"};
        appendTaskMetric("2", path, vals2, heads2);
        std::cout << "[2]" << std::endl;

        std::cout << "Граф " << ++current << " из " << graphFiles.size() << " обработан." << std::endl;
    }

    return 0;
}
