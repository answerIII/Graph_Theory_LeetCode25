#include <iostream>
#include "classes/Graph.h"
#include "classes/Parser.h"
#include "functions/parserf.h"
#include "functions/CsvParser.h"
#include "functions/getOutput.h"

int chooseAlgo() {
    int algo = 0;
    std::cout << "\nВыберите задание:\n";
    std::cout << "1 - Анализ структуры сети\n";
    std::cout << "2 - Вычисление расстояний между вершинами сети\n";
    std::cout << "Введите номер: ";
    std::cin >> algo;
    while (algo != 1 && algo != 2) {
        std::cout << "Введено неверное число! Попробуйте заново: ";
        std::cin >> algo;

    }
    return algo;
}
int main(const int argc, const char *argv[]) {
    validateArgs(argc, argv);
        std::string dataset_root = "datasets";
    std::vector<GraphFile> graph_files;

    for (const auto& dir_entry : fs::recursive_directory_iterator(dataset_root)) {
        if (dir_entry.is_regular_file()) {
            std::string path = dir_entry.path().string();
            std::string name = dir_entry.path().filename().string();
            std::string category = dir_entry.path().parent_path().filename().string();
            graph_files.push_back({ name, path, category });
        }
    }

    std::map<std::string, std::vector<GraphFile>> category_map;
    for (const auto& file : graph_files) {
        category_map[file.category].push_back(file);
    }

    std::cout << "\nВыберите режим работы:\n";
    std::cout << "1 - Выбрать один файл\n";
    std::cout << "2 - Обработать все файлы из одной категории\n";
    std::cout << "3 - Обработать ВСЕ файлы\n";
    std::cout << "Ваш выбор: ";

    int mode;
    std::cin >> mode;

    if (mode == 1) {
        std::cout << "\nВыберите файл:\n";
        for (size_t i = 0; i < graph_files.size(); ++i) {
            std::cout << i + 1 << " - [" << graph_files[i].category << "] " << graph_files[i].display_name << "\n";
        }
        int choice;
        std::cout << "Введите номер: ";
        std::cin >> choice;

        if (choice < 1 || choice > graph_files.size()) {
            std::cerr << "Некорректный выбор!\n";
            return 1;
        }
        int algo = chooseAlgo();

        get_results(graph_files[choice - 1].path.c_str(), graph_files[choice - 1].category, algo);

    } else if (mode == 2) {
        std::vector<std::string> categories;
        std::cout << "\nВыберите категорию:\n";
        int idx = 1;
        for (const auto& pair : category_map) {
            std::cout << idx << " - " << pair.first << "\n";
            categories.push_back(pair.first);
            ++idx;
        }

        int choice;
        std::cout << "Введите номер категории: ";
        std::cin >> choice;

        if (choice < 1 || choice > categories.size()) {
            std::cerr << "Некорректный выбор!\n";
            return 1;
        }
        int algo = chooseAlgo();

        std::string selected_cat = categories[choice - 1];
        std::cout << "\nЗапускаем обработку всех файлов из: " << selected_cat << "\n";

        for (const auto& file : category_map[selected_cat]) {
            std::cout << "→ Обработка: " << file.display_name << "\n";
            get_results(file.path.c_str(), file.category,algo);
        }

    } else if (mode == 3) {
        int algo = chooseAlgo();
        std::cout << "\nЗапускаем обработку всех файлов...\n";
        for (const auto& file : graph_files) {
            std::cout << "→ Обработка: " << file.display_name << " [" << file.category << "]\n";
            get_results(file.path.c_str(), file.category,algo);
        }
        std::cout << line << "\nЗапускаем объединение CSV-файлов...\n";
        merge_csv_files_in_folder("output_data");
    } else {
        std::cerr << "Неверный режим!\n";
        return 1;
    }
    std::cout << line << std::endl;
    std::cout << "Обработка завершена\n";

    return 0;
}
