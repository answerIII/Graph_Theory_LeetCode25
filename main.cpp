#include "Graph.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace std::chrono;

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

        // // Плотность
        // auto startDensity = high_resolution_clock::now();
        // double density = g.getDensity();
        // auto endDensity = high_resolution_clock::now();
        // auto densityTime = duration_cast<milliseconds>(endDensity - startDensity);
        // cout << "Плотность: " << formatScientific(density) << " (" << densityTime.count() << " ms)\n";

        // // WCC
        // auto startWCC = high_resolution_clock::now();
        // int wcc = g.countWeaklyConnectedComponents();
        // double wccRatio = g.getWCCRatio() * 100.0;
        // auto endWCC = high_resolution_clock::now();
        // auto wccTime = duration_cast<milliseconds>(endWCC - startWCC);
        // cout << "WCC: " << wcc << " (" << wccTime.count() << " ms), " << formatDouble(wccRatio) << " %\n";

        // // SCC
        // if (isDirected) {
        //     auto startSCC = high_resolution_clock::now();
        //     int scc = g.countStronglyConnectedComponents();
        //     double sccRatio = g.getLargestSCCRatio() * 100.0;
        //     auto endSCC = high_resolution_clock::now();
        //     auto sccTime = duration_cast<milliseconds>(endSCC - startSCC);
        //     cout << "SCC: " << scc << " (" << sccTime.count() << " ms), " << formatDouble(sccRatio) << " %\n";
        // } else {
        //     cout << "SCC: -\n";
        // }

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
