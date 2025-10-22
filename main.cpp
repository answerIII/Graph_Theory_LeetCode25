#include "Graph/Graph.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include <string>

namespace fs = std::filesystem;
using Clock = std::chrono::high_resolution_clock;
int GLOBAL_TU_PARAM = 6;
bool isTestGraphPath(const std::string& graphPath);

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

fs::path findRootWithDatasets() {
    fs::path p = fs::current_path();
    fs::path start = p;
    while (true) {
        if (fs::exists(p / "datasets") && fs::is_directory(p / "datasets")) {
            return p;
        }
        if (!p.has_parent_path() || p.parent_path() == p) {
            return start;
        }
        p = p.parent_path();
    }
}

void ensureOutputsForMode(bool testsMode) {
    if (testsMode) {
        fs::create_directories("testsResults");
        fs::path unifiedCsv = fs::path("testsResults") / "all_metrics.csv";
        if (fs::exists(unifiedCsv)) {
            fs::remove(unifiedCsv);
        }
    } else {
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
    if (isTestGraphPath(graphPath)) return; // do not create per-graph CSVs for tests
    std::string graphName = fs::path(graphPath).filename().string();
    std::ofstream outGraph("results/graphs/" + graphName + ".csv", std::ios::app);
    outGraph << metric << "," << value << "\n";
    outGraph.close();
}

void appendTaskMetric(const std::string& task, const std::string& graphPath, const std::vector<std::string>& values, const std::vector<std::string>& headers) {
    if (isTestGraphPath(graphPath)) return; // do not create per-task CSVs for tests
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

bool isTestGraphPath(const std::string& graphPath) {
    return graphPath.find("/tests/") != std::string::npos || graphPath.find("\\tests\\") != std::string::npos;
}

void appendUnifiedRow(const std::string& graphPath,
                      const std::unordered_map<std::string, std::string>& m) {
    if (!isTestGraphPath(graphPath)) return;
    fs::create_directories("testsResults");
    std::string outPath = "testsResults/all_metrics.csv";
    static bool headerWritten = false;
    static std::vector<std::string> headers = {
        "file","is_test",
        "vertex count","edge count",
        "density",
        "largest WCC size",
        "diameter1",
        "triangle count",
        "distanceFromMaxVertex(t)","localClustering(u)","t_param","u_param"
    };
    std::ofstream out(outPath, std::ios::app);
    if (!headerWritten) {
        for (size_t i = 0; i < headers.size(); ++i) {
            out << headers[i];
            if (i + 1 < headers.size()) out << ",";
        }
        out << "\n";
        headerWritten = true;
    }
    for (size_t i = 0; i < headers.size(); ++i) {
        if (i == 0) {
            out << graphPath;
        } else if (i == 1) {
            out << (isTestGraphPath(graphPath) ? "1" : "0");
        } else {
            auto it = m.find(headers[i]);
            if (it != m.end()) out << it->second;
        }
        if (i + 1 < headers.size()) out << ",";
    }
    out << "\n";
    out.close();
}

int main() {
     auto start_time = std::chrono::system_clock::now();
    std::time_t start_time_t = std::chrono::system_clock::to_time_t(start_time);

    fs::path projectRoot = findRootWithDatasets();
    try {
        fs::current_path(projectRoot);
        std::cout << "Рабочая директория установлена: " << fs::current_path() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Не удалось установить рабочую директорию: " << e.what() << std::endl;
    }
    std::cout << "Программа начала работу: " 
              << std::put_time(std::localtime(&start_time_t), "%Y-%m-%d %H:%M:%S") 
              << std::endl;
    auto graphFiles = collectGraphFiles("datasets");

    std::cout << "Какие графы считать? tests(t)/обычные(o) [o]: ";
    std::string ans;
    std::getline(std::cin, ans);
    bool testsMode = !ans.empty() && (ans[0] == 't' || ans[0] == 'T');
    ensureOutputsForMode(testsMode);

    std::vector<std::string> processFiles;
    processFiles.reserve(graphFiles.size());
    for (const auto& pth : graphFiles) {
        bool isTest = isTestGraphPath(pth);
        if (testsMode ? isTest : !isTest) processFiles.push_back(pth);
    }
    if (testsMode) {
        std::sort(processFiles.begin(), processFiles.end());
    }
    const size_t total = processFiles.size();
    if (total == 0) {
        std::cout << (testsMode ? "Нет тестовых графов." : "Нет обычных графов.") << std::endl;
        return 0;
    }


    int current = 0;
    for (const auto& path : processFiles) {
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

        bool isTest = isTestGraphPath(path);

        std::unordered_map<std::string, std::string> UM;
        UM["load time"] = loadTime;

        if (isTest) {
            int vc = g.getVertexCount();
            UM["vertex count"] = std::to_string(vc);
            int ec = g.getEdgeCount();
            UM["edge count"] = std::to_string(ec);

            double dens = g.getDensity();
            UM["density"] = std::to_string(dens);

            int vc_int = vc;
            int wcc = g.countWeaklyConnectedComponents(); (void)wcc;
            double wccr = g.getWCCRatio();
            int largestWCC = static_cast<int>(wccr * vc_int);
            UM["largest WCC size"] = std::to_string(largestWCC);

            int d1 = g.estimateDiameterDoubleSweep().diameter;
            UM["diameter1"] = std::to_string(d1);

            g.countTriangles(); long long tri = g.getTriangles();
            UM["triangle count"] = std::to_string(tri);

            int nverts = g.getVertexCount();
            int tParam = std::min(GLOBAL_TU_PARAM, std::max(0, nverts - 1));
            int distMax = g.distanceFromMaxVertex(tParam);
            double lccu = g.localClusteringCoefficient(tParam);
            UM["distanceFromMaxVertex(t)"] = std::to_string(distMax);
            UM["localClustering(u)"] = std::to_string(lccu);
            UM["t_param"] = std::to_string(tParam);
            UM["u_param"] = std::to_string(tParam);

            appendUnifiedRow(path, UM);
            std::cout << "[tests subset done]" << std::endl;
            std::cout << "Граф " << ++current << " из " << total << " обработан." << std::endl;
            continue;
        }

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

        // // === Задание 1A2 ===
        auto t_d1 = Clock::now(); int d1 = g.estimateDiameterDoubleSweep().diameter; std::string td1 = formatDuration(Clock::now() - t_d1);
        appendGraphMetric(path, "оценка диаметра (double sweep)", std::to_string(d1));
        appendGraphMetric(path, "время оценки double sweep", td1);

        auto t_dist = Clock::now(); auto [diam, p90_1] = g.distanceStatsRandomPairs(); std::string tdist = formatDuration(Clock::now() - t_dist);
        appendGraphMetric(path, "оценка диаметра (рандомные пары - 500)", std::to_string(diam));
        appendGraphMetric(path, "90 процентиль (рандомные пары - 500)", std::to_string(p90_1));
        appendGraphMetric(path, "время подсчета дистанций (рандомные пары - 500)", tdist);

        auto t_dist_2 = Clock::now(); auto [diam_1, p90_2] = g.distanceStatsRandomPairs(1000); std::string tdist2 = formatDuration(Clock::now() - t_dist_2);
        appendGraphMetric(path, "оценка диаметра (рандомные пары - 1000)", std::to_string(diam));
        appendGraphMetric(path, "90 процентиль (рандомные пары - 1000)", std::to_string(p90_1));
        appendGraphMetric(path, "время подсчета дистанций (рандомные пары - 1000)", tdist);

        auto t_snow = Clock::now(); auto [d2, p90] = g.snowballDiameterAndP90(); std::string tsnow = formatDuration(Clock::now() - t_snow);
        appendGraphMetric(path, "оценка диаметра (snowball)", std::to_string(d2));
        appendGraphMetric(path, "90 процентиль расстояний (snowball)", std::to_string(p90));
        appendGraphMetric(path, "время подсчета дистанций (snowball)", tsnow);

        std::vector<std::string> vals1A2 = {path, loadTime, std::to_string(d1), td1, std::to_string(diam), tdist, std::to_string(p90_1), tdist, std::to_string(d2), tsnow, std::to_string(p90), tsnow};
        std::vector<std::string> heads1A2 = {"file", "load time", "diameter1", "diameter1 time", "diameter2(500)", "diameter2(500) time", "p90(500)", "p90(500) time", "diameter_snowball", "diameter_snowball time", "p90_snowball", "p90_snowball time"};
        appendTaskMetric("1A2", path, vals1A2, heads1A2);
        std::cout << "[1A2]" << std::endl;

        // === Задание 1A3 ===
        auto t_tri = Clock::now(); g.countTriangles(); long long tri = g.getTriangles(); std::string ttri = formatDuration(Clock::now() - t_tri);
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
        fs::path outDir = "GraphsCSVTables";
        if (!fs::exists(outDir))
            fs::create_directory(outDir);

        std::string base = path.substr(path.find_last_of('/') + 1);
        fs::path csvPath = outDir / (base + "_deg_dist.csv");

        {
            std::ofstream csv(csvPath);
            csv << "k,count,prob\n";
            const auto& hist = g.degreeHistogram();
            for (size_t k = 0; k < hist.size(); ++k)
                if (hist[k])
                    csv << k << ',' << hist[k] << ','
                        << double(hist[k]) / g.getVertexCount() << '\n';
        }
        std::cout << "Гистограмма степеней сохранена в " << csvPath.string() << '\n';

        // === Задание 1B ===
        std::vector<std::string> vals1B = {path, loadTime};
        std::vector<std::string> heads1B = {"file", "load time"};

        for (int perc = 10; perc <= 90; perc += 10) {
            auto t_rnd = Clock::now();
            double r_rnd = g.ratioAfterRemoval(perc, false);
            std::string tr_rnd = formatDuration(Clock::now() - t_rnd);

            appendGraphMetric(path, "random removal ratio " + std::to_string(perc) + "%", std::to_string(r_rnd));
            appendGraphMetric(path, "random time " + std::to_string(perc) + "%", tr_rnd);

            auto t_tar = Clock::now();
            double r_tar = g.ratioAfterRemoval(perc, true);
            std::string tr_tar = formatDuration(Clock::now() - t_tar);

            appendGraphMetric(path, "targeted removal ratio " + std::to_string(perc) + "%", std::to_string(r_tar));
            appendGraphMetric(path, "targeted time " + std::to_string(perc) + "%", tr_tar);

            vals1B.push_back(std::to_string(r_rnd));
            vals1B.push_back(tr_rnd);
            vals1B.push_back(std::to_string(r_tar));
            vals1B.push_back(tr_tar);

            heads1B.push_back("random removal ratio " + std::to_string(perc) + "%");
            heads1B.push_back("random time " + std::to_string(perc) + "%");
            heads1B.push_back("targeted removal ratio " + std::to_string(perc) + "%");
            heads1B.push_back("targeted time " + std::to_string(perc) + "%");
        }

        appendTaskMetric("1B", path, vals1B, heads1B);
        std::cout << "[1B]" << std::endl;

        // === Задание 2 ===
        auto t_lm = Clock::now(); g.precomputeLandmarks(64, Graph::LandmarkSelect::Random); std::string tlm = formatDuration(Clock::now() - t_lm);
        appendGraphMetric(path, "время препроцессинга landmarks", tlm);

        int n = g.getVertexCount();
        std::mt19937 rng(123);
        std::uniform_int_distribution<int> dist(0, n - 1);
        int numPairs = 100, sumDist = 0, validPairs = 0;
        int absDiff = 0, measuredPairs = 0;

        auto t_appr = Clock::now();
        for (int j = 0; j < numPairs; ++j) {
            int s = dist(rng), t = dist(rng);
            while (s == t) t = dist(rng);

            int approx = g.landmarkBasicDistance(s, t);
            int exact  = g.landmarkBFSDistance(s, t);

            if (approx >= 0 && exact >= 0) {
                sumDist += approx;
                ++validPairs;
                absDiff += std::abs(approx - exact);
                ++measuredPairs;
            }
        }
        std::string tappr = formatDuration(Clock::now() - t_appr);
        double avg = validPairs > 0 ? static_cast<double>(sumDist) / validPairs : -1.0;
        double avgError = measuredPairs > 0 ? static_cast<double>(absDiff) / measuredPairs : -1.0;

        appendGraphMetric(path, "количество допустимых пар", std::to_string(validPairs));
        appendGraphMetric(path, "среднее расстояние по landmarks", std::to_string(avg));
        appendGraphMetric(path, "время подсчета по landmarks", tappr);
        appendGraphMetric(path, "средняя погрешность landmarks vs BFS", std::to_string(avgError));

        std::vector<std::string> vals2 = {path, loadTime, std::to_string(validPairs), tappr, std::to_string(avg), tappr, tlm, std::to_string(avgError)};
        std::vector<std::string> heads2 = {"file", "load time", "valid pairs", "pairs time", "avg landmark dist", "avg dist time", "landmark preprocess time", "avg error (landmarks vs bfs)"};
        appendTaskMetric("2", path, vals2, heads2);
        std::cout << "[2]" << std::endl;

        {
            std::unordered_map<std::string, std::string> UM;
            UM["load time"] = loadTime;
            UM["vertex count"] = std::to_string(vc); UM["vertex time"] = tvc;
            UM["edge count"] = std::to_string(ec); UM["edge time"] = tec;
            UM["density"] = std::to_string(dens); UM["density time"] = tdens;
            UM["WCC count"] = std::to_string(wcc); UM["WCC time"] = twcc;
            UM["SCC count"] = std::to_string(scc); UM["SCC time"] = tscc;
            UM["WCC ratio"] = std::to_string(wccr); UM["WCC ratio time"] = twccr;
            UM["SCC ratio"] = std::to_string(sccr); UM["SCC ratio time"] = tsccr;
            UM["diameter1"] = std::to_string(d1); UM["diameter1 time"] = td1;
            UM["diameter2(500)"] = std::to_string(diam); UM["diameter2(500) time"] = tdist; UM["p90(500)"] = std::to_string(p90_1); UM["p90(500) time"] = tdist;
            UM["diameter_snowball"] = std::to_string(d2); UM["diameter_snowball time"] = tsnow; UM["p90_snowball"] = std::to_string(p90); UM["p90_snowball time"] = tsnow;
            UM["triangle count"] = std::to_string(tri); UM["triangle time"] = ttri;
            UM["avg clustering"] = std::to_string(acc); UM["avg clustering time"] = tacc;
            UM["global clustering"] = std::to_string(gcc); UM["global clustering time"] = tgcc;
            UM["avg clustering in WCC"] = std::to_string(awcc); UM["avg clustering in WCC time"] = tawcc;
            UM["min deg"] = std::to_string(mindeg); UM["min deg time"] = tdeg;
            UM["max deg"] = std::to_string(maxdeg); UM["max deg time"] = tdeg;
            UM["avg deg"] = std::to_string(avgdeg); UM["avg deg time"] = tdeg;
            UM["valid pairs"] = std::to_string(validPairs); UM["pairs time"] = tappr;
            UM["avg landmark dist"] = std::to_string(avg); UM["avg dist time"] = tappr;
            UM["landmark preprocess time"] = tlm; UM["avg error (landmarks vs bfs)"] = std::to_string(avgError);

            int nverts = g.getVertexCount();
            int tParam = std::min(GLOBAL_TU_PARAM, std::max(0, nverts - 1));
            int distMax = g.distanceFromMaxVertex(tParam);
            double lccu = g.localClusteringCoefficient(tParam);
            UM["distanceFromMaxVertex(t)"] = std::to_string(distMax);
            UM["localClustering(u)"] = std::to_string(lccu);
            UM["t_param"] = std::to_string(tParam);
            UM["u_param"] = std::to_string(tParam);

            appendUnifiedRow(path, UM);
        }
        std::cout << "Граф " << ++current << " из " << total << " обработан." << std::endl;
    }

      auto end_time = std::chrono::system_clock::now();
    std::time_t end_time_t = std::chrono::system_clock::to_time_t(end_time);
    std::chrono::duration<double> elapsed_seconds = end_time - start_time;
        
    std::cout << "Программа завершила работу: " 
              << std::put_time(std::localtime(&end_time_t), "%Y-%m-%d %H:%M:%S") 
              << std::endl;
    std::cout << "Общее время выполнения: " << elapsed_seconds.count() << " секунд" << std::endl;

    return 0;
}
