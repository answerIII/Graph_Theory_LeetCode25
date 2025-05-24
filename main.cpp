#include "Graph.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;
using namespace std::chrono;

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
            for (int i = 0; i < allGraphs.size(); ++i)
                selectedIndexes.push_back(i);
            break;
        }

        istringstream iss(inputLine);
        int num;
        bool hasError = false;
        selectedIndexes.clear();

        while (iss >> num) {
            if (num >= 1 && num <= (int)allGraphs.size()) {
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

    bool runWCC = false, runSCC = false, runDensity = false;
    char input;

    cout << "\nЗапустить анализ плотности? (y/n): "; cin >> input;
    runDensity = (input == 'y' || input == 'Y');

    cout << "Запустить компоненты слабой связности (WCC)? (y/n): "; cin >> input;
    runWCC = (input == 'y' || input == 'Y');

    cout << "Запустить компоненты сильной связности (SCC)? (y/n): "; cin >> input;
    runSCC = (input == 'y' || input == 'Y');

    bool measureTime = false;
    cout << "Измерять время выполнения? (y/n): "; cin >> input;
    measureTime = (input == 'y' || input == 'Y');

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "\n====================================================================================================================\n";
    cout << setw(35) << left << "Граф"
        << setw(10) << right << "Вершин"
        << setw(10) << "Рёбер";

    if (runDensity) cout << setw(18) << "Плотность";
    if (runWCC)     cout << setw(12) << "WCC" << setw(12) << "WCC %";
    if (runSCC)     cout << setw(12) << "SCC" << setw(12) << "SCC %";
    if (measureTime) cout << setw(12) << "Вр. загрузки";

    cout << "\n====================================================================================================================\n";

    for (int idx : selectedIndexes) {
        string path = allGraphs[idx];
        string ext = path.substr(path.find_last_of('.'));
        string format = formatMap.count(ext) ? formatMap[ext] : "";

        if (format.empty()) {
            cout << setw(35) << left << path << " [!] Неизвестный формат\n";
            continue;
        }

        Graph g;
        auto startLoad = high_resolution_clock::now();
        g.loadFromFile(path, format);
        auto endLoad = high_resolution_clock::now();
        auto loadTime = duration_cast<milliseconds>(endLoad - startLoad);

        bool isDirected = g.getDirected();
        int vCount = g.getVertexCount();
        int eCount = g.getEdgeCount();

        cout << setw(35) << left << path.substr(path.find_last_of('/') + 1)
            << setw(10) << right << vCount
            << setw(10) << eCount;

        if (runDensity) {
            double d1 = g.getDensity();
            cout << setw(18) << scientific << setprecision(6) << d1;
        }

        if (runWCC) {
            int wcc = g.countWeaklyConnectedComponents();
            double wccRatio = g.getWCCRatio() * 100.0;
            cout << setw(12) << fixed << wcc
                << setw(12) << fixed << setprecision(2) << wccRatio;
        }

        if (runSCC && isDirected) {
            int scc = g.countStronglyConnectedComponents();
            double sccRatio = g.getLargestSCCRatio() * 100.0;
            cout << setw(12) << fixed << scc
                << setw(12) << fixed << setprecision(2) << sccRatio;
        } else if (runSCC && !isDirected) {
            cout << setw(12) << "-" << setw(12) << "-";
        }

        if (measureTime)
            cout << setw(12) << loadTime.count();

        cout << "\n";
    }
    return 0;
}
