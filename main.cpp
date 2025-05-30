#include "Graph.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

string formatScientific(double value) {
    stringstream ss;
    ss << scientific << setprecision(4) << value;
    return ss.str();
}

string formatDouble(double value) {
    stringstream ss;
    ss << fixed << setprecision(2) << value;
    return ss.str();
}

int main() {
    vector<string> allGraphs = {
        "datasets/directed/soc-wiki-Vote.mtx",
        "datasets/directed/web-Google.txt",
        "datasets/directed/web-NotreDame.txt",
        "datasets/directed/web-Stanford.txt",
        "datasets/directed/Wiki-Vote.txt",
        "datasets/undirected/CA-AstroPh.txt",
        "datasets/undirected/ca-coauthors-dblp.txt",
        "datasets/undirected/CA-GrQc.txt",
        "datasets/undirected/Email-EuAll.txt",
        "datasets/undirected/musae_git_edges.csv",
        "datasets/very_large_graphs/com-orkut.ungraph.txt",
        "datasets/very_large_graphs/com-youtube.ungraph.txt",
        "datasets/very_large_graphs/vk.csv"
    };

    map<string, string> formatMap = {
        {".txt", "txt"},
        {".mtx", "mtx"},
        {".csv", "csv"}
    };

    cout << "=====================================\n";
    cout << "  Граф-анализатор (интерактивный режим)\n";
    cout << "=====================================\n\n";

    cout << "Доступные графы:\n";
    for (size_t i = 0; i < allGraphs.size(); ++i) {
        cout << i + 1 << ") " << allGraphs[i] << "\n";
    }

    vector<int> selectedIndexes;
    string inputLine;

    while (selectedIndexes.empty()) {
        cout << "\nВведите номера графов через пробел (или 'all' для всех): ";
        cout.flush();
        getline(cin, inputLine);

        if (inputLine == "all") {
            for (int i = 0; i < static_cast<int>(allGraphs.size()); ++i)
                selectedIndexes.push_back(i);
            break;
        }

        istringstream iss(inputLine);
        int num;
        bool hasError = false;
        selectedIndexes.clear();

        while (iss >> num) {
            if (num >= 1 && num <= static_cast<int>(allGraphs.size())) {
                selectedIndexes.push_back(num - 1);
            } else {
                cout << "[!] Неверный номер: " << num << "\n";
                hasError = true;
                break;
            }
        }

        if (selectedIndexes.empty() || hasError) {
            selectedIndexes.clear();
            cout << "[!] Повторите ввод корректно.\n";
        }
    }

    const std::vector<double> removalPerc = {10,20,30,40,50,60,70,80,90};

    for (int idx : selectedIndexes) {
        string path = allGraphs[idx];
        string ext = path.substr(path.find_last_of('.'));
        string format = formatMap.count(ext) ? formatMap[ext] : "";

        if (format.empty()) {
            cout << "Неизвестный формат файла: " << path << endl;
            continue;
        }

        cout << "-------------------------------\n";
        cout << "Граф: " << path.substr(path.find_last_of('/') + 1) << "\n";

        // Загрузка графа
        auto startLoad = high_resolution_clock::now();
        Graph g;
        g.loadFromFile(path, format);
        auto endLoad = high_resolution_clock::now();
        auto loadTime = duration_cast<milliseconds>(endLoad - startLoad);
        cout << "Загрузка: " << loadTime.count() << " ms\n";

        bool isDirected = g.getDirected();
        int vCount = g.getVertexCount();
        int eCount = g.getEdgeCount();

        cout << "Вершин: " << vCount << "\n";
        cout << "Рёбер: " << eCount << "\n";

        // Плотность
        auto startDensity = high_resolution_clock::now();
        double density = g.getDensity();
        auto endDensity = high_resolution_clock::now();
        auto densityTime = duration_cast<milliseconds>(endDensity - startDensity);

        double fractional = abs(density - floor(density));
        bool hasNonZeroFraction = (fractional * 1e6 - floor(fractional * 1e6)) > 1e-12;

        if (hasNonZeroFraction) {
            cout << "Плотность: " << fixed << setprecision(6) << density << " (" << densityTime.count() << " ms)\n";
        } else {
            cout << "Плотность: " << fixed << setprecision(0) << density << " (" << densityTime.count() << " ms)\n";
        }

        // WCC
        auto startWCC = high_resolution_clock::now();
        int wcc = g.countWeaklyConnectedComponents();
        double wccRatio = g.getWCCRatio() * 100.0;
        auto endWCC = high_resolution_clock::now();
        auto wccTime = duration_cast<milliseconds>(endWCC - startWCC);
        cout << "WCC: " << wcc << " (" << wccTime.count() << " ms), " << formatDouble(wccRatio) << " %\n";

        // SCC
        if (isDirected) {
            auto startSCC = high_resolution_clock::now();
            int scc = g.countStronglyConnectedComponents();
            double sccRatio = g.getLargestSCCRatio() * 100.0;
            auto endSCC = high_resolution_clock::now();
            auto sccTime = duration_cast<milliseconds>(endSCC - startSCC);
            cout << "SCC: " << scc << " (" << sccTime.count() << " ms), " << formatDouble(sccRatio) << " %\n";
        } else {
            cout << "SCC: -\n";
        }

        // average cluster coefficient
        auto t0 = high_resolution_clock::now();
        double Cl = g.averageClusteringLargestWCC();
        auto t1 = high_resolution_clock::now();
        cout << "⟨C_l⟩ (Largest WCC): " << fixed << setprecision(6) << Cl << " (" << duration_cast<milliseconds>(t1-t0).count() << " ms)\n";

        auto tri = g.countTriangles();
        auto avgC = g.averageClusteringCoefficient();
        auto globC = g.globalClusteringCoefficient();

        cout << "Количество треугольников: " << tri << "\n";
        cout << "Средний коэффициент кластеризации: " << fixed << setprecision(6) << avgC << "\n";
        cout << "Глобальный коэффициент кластеризации: " << fixed << setprecision(6) << globC << "\n";

        //min, max, average degree
        auto ds = g.getDegreeStats();
        cout << "Мин. степень: "  << ds.minDeg  << "\n";
        cout << "Макс. степень: " << ds.maxDeg  << "\n";
        cout << "Средняя степень: " << fixed << setprecision(2) << ds.avgDeg << "\n";

        // Save in CSV (k,count,P(k)) for images
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

        fs::path vfcDir = "VertexFractionChange";
        if (!fs::exists(vfcDir)) fs::create_directory(vfcDir);

        base = path.substr(path.find_last_of('/') + 1);
        fs::path csvVFC = vfcDir / (base + "_vfc.csv");

        {
            std::ofstream csv(csvVFC);
            csv << "percent,random,top_degree\n";

            for (double p : removalPerc) {
                double ratioRand = g.ratioAfterRemoval(p, false);
                double ratioTarget = g.ratioAfterRemoval(p, true);

                csv << p << ','
                    << std::fixed << std::setprecision(4) << ratioRand << ','
                    << std::fixed << std::setprecision(4) << ratioTarget << '\n';
            }
        }

        std::cout << "Таблица изменений долей вершин по процентам сохранена в " << vfcDir.string() << '\n';

        using namespace std::chrono;
        int n = g.getVertexCount();

        int trials = 100;  
        std::mt19937_64 rng(12345);
        std::uniform_int_distribution<int> vid(0, n-1);
        std::vector<std::pair<int,int>> queries;
        queries.reserve(trials);
        for (int i = 0; i < trials; ++i) {
            int s = vid(rng), t = vid(rng);
            while (t == s) t = vid(rng);
            queries.emplace_back(s, t);
        }

        std::vector<int> Ks = {64, 128, 256};

        vector<pair<string, Graph::LandmarkSelect>> methods = {
            {"Random", Graph::LandmarkSelect::Random},
            {"HighestDegree", Graph::LandmarkSelect::HighestDegree}
        };

        for (auto &m : methods) {
            const string &methodName = m.first;
            auto methodSel = m.second;

            cout << "\n### Способ: " << methodName << " ###\n";

            for (int K : Ks) {
                cout << "\nLandmarks-Basic K=" << K << " (" << methodName << ")\n";

                auto t0 = high_resolution_clock::now();
                g.precomputeLandmarks(K, methodSel);
                auto t1 = high_resolution_clock::now();
                cout << "Precompute BFS: " << duration_cast<milliseconds>(t1-t0).count() << " ms\n";

                double sumRelErr = 0;
                double sumExactT = 0, sumApproxT = 0;

                for (auto [s,t] : queries) {
                    auto te0 = high_resolution_clock::now();
                    int exact = g.exactDistance(s, t);
                    auto te1 = high_resolution_clock::now();
                    sumExactT += duration_cast<microseconds>(te1-te0).count();

                    auto ta0 = high_resolution_clock::now();
                    int approx = g.landmarkBasicDistance(s, t);
                    auto ta1 = high_resolution_clock::now();
                    sumApproxT += duration_cast<microseconds>(ta1-ta0).count();

                    if (exact > 0 && approx > 0)
                        sumRelErr += std::abs(double(approx - exact)) / exact;
                }

                double avgErr = sumRelErr / trials * 100.0;
                double avgExact = sumExactT / trials;
                double avgApprox = sumApproxT / trials;

                cout << fixed << setprecision(2) << "Avg rel error: " 
                    << avgErr << " %\n" << "Avg exact BFS time: " << avgExact << " μs\n" 
                    << "Avg landmark time: " << avgApprox << " μs\n";
            }
        }

        // auto startDS = high_resolution_clock::now();
        // int diamDS = g.estimateDiameterDoubleSweep();
        // auto endDS = high_resolution_clock::now();
        // auto dsTime = duration_cast<milliseconds>(endDS - startDS);
        // cout << "Диаметр Double Sweep: " << diamDS << " (" << dsTime.count() << " ms)\n";

        // auto startRP = high_resolution_clock::now();
        // auto [diamRP, p90RP] = g.estimateDiameterRandomPairs(500);
        // auto endRP = high_resolution_clock::now();
        // auto rpTime = duration_cast<milliseconds>(endRP - startRP);
        // cout << "Диаметр Random Pairs: " << diamRP << " (" << rpTime.count() << " ms)\n";
        // cout << "P90 Random Pairs: " << formatDouble(p90RP) << " (" << rpTime.count() << " ms)\n";
        // cout << endl;
    }

    return 0;
}
