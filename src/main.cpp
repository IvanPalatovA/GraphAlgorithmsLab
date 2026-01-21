#include <iostream>
#include <chrono>
#include <string>
#include <cstdlib>

#include "graph_algorithms.h"
#include "tictactoe_graph.h"

// ------------------------------------------------------------
// Консольный пользовательский интерфейс
// ------------------------------------------------------------

void print_menu() {
    std::cout << "\n=== Лабораторная работа 3: алгоритмы на графах ===\n";
    std::cout << "1. Создать граф вручную\n";
    std::cout << "2. Загрузить граф из файла\n";
    std::cout << "3. Сгенерировать случайный граф\n";
    std::cout << "4. Показать текущий граф\n";
    std::cout << "5. Запустить алгоритм Дейкстры\n";
    std::cout << "6. Запустить алгоритм Беллмана–Форда\n";
    std::cout << "7. Сравнить алгоритмы на текущем графе\n";
    std::cout << "8. Сохранить результаты последнего сравнения в CSV\n";
    std::cout << "9. Запустить встроенные тесты\n";
    std::cout << "10. Крестики-нолики: рекомендовать ход ноликами\n";
    std::cout << "11. Крестики-нолики: игра с компьютером (pygame)\n";
    std::cout << "12. Визуализировать текущий граф (Python, векторы)\n";
    std::cout << "0. Выход\n";
    std::cout << "Выберите пункт меню: ";
}

int main() {
    Graph current_graph;
    bool has_graph = false;
    DynamicArray<BenchmarkRecord> last_benchmarks;

    while (true) {
        print_menu();
        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cout << "Ошибка ввода. Завершение работы.\n";
            break;
        }

        if (choice == 0) {
            std::cout << "Выход.\n";
            break;
        }

        switch (choice) {
        case 1: {
            int n, m;
            int directed_flag;
            std::cout << "Введите число вершин: ";
            std::cin >> n;
            std::cout << "Граф ориентированный? (1 - да, 0 - нет): ";
            std::cin >> directed_flag;
            std::cout << "Введите число рёбер: ";
            std::cin >> m;

            Graph g(n, directed_flag != 0);
            std::cout << "Введите рёбра в формате: u v w\n";
            for (int i = 0; i < m; ++i) {
                Vertex u, v;
                Weight w;
                std::cin >> u >> v >> w;
                try {
                    g.add_edge(u, v, w);
                } catch (const std::exception& ex) {
                    std::cout << "Ошибка: " << ex.what() << "\n";
                    --i; // позволяем ввести ребро заново
                }
            }
            current_graph = std::move(g);
            has_graph = true;
            std::cout << "Граф создан.\n";
            break;
        }
        case 2: {
            std::string filename;
            std::cout << "Введите имя файла для загрузки: ";
            std::cin >> filename;
            if (load_graph_from_file(current_graph, filename)) {
                has_graph = true;
                std::cout << "Граф успешно загружен.\n";
            } else {
                std::cout << "Не удалось загрузить граф из файла.\n";
            }
            break;
        }
        case 3: {
            int n;
            double p;
            double min_w, max_w;
            int directed_flag;
            std::cout << "Введите число вершин: ";
            std::cin >> n;
            std::cout << "Граф ориентированный? (1 - да, 0 - нет): ";
            std::cin >> directed_flag;
            std::cout << "Введите вероятность ребра (0..1): ";
            std::cin >> p;
            std::cout << "Минимальный вес: ";
            std::cin >> min_w;
            std::cout << "Максимальный вес: ";
            std::cin >> max_w;
            try {
                current_graph =
                    generate_random_graph(n, p, min_w, max_w, directed_flag != 0);
                has_graph = true;
                std::cout << "Граф сгенерирован.\n";
            } catch (const std::exception& ex) {
                std::cout << "Ошибка: " << ex.what() << "\n";
            }
            break;
        }
        case 4: {
            if (!has_graph) {
                std::cout << "Сначала создайте или загрузите граф.\n";
            } else {
                print_graph(current_graph);
            }
            break;
        }
        case 5: {
            if (!has_graph) {
                std::cout << "Сначала создайте или загрузите граф.\n";
                break;
            }
            Vertex s, t;
            std::cout << "Введите начальную вершину: ";
            std::cin >> s;
            std::cout << "Введите конечную вершину (-1, если не нужно восстанавливать путь): ";
            std::cin >> t;

            auto start = std::chrono::steady_clock::now();
            auto res = dijkstra(current_graph, s);
            auto finish = std::chrono::steady_clock::now();
            double time_ms =
                std::chrono::duration_cast<std::chrono::microseconds>(finish - start)
                    .count() /
                1000.0;

            std::cout << "Алгоритм Дейкстры завершён за " << time_ms << " мс.\n";
            print_distances(res);

            if (t >= 0) {
                auto path = restore_path(s, t, res.parent);
                std::cout << "Путь от " << s << " до " << t << ": ";
                print_path(path);
            }
            break;
        }
        case 6: {
            if (!has_graph) {
                std::cout << "Сначала создайте или загрузите граф.\n";
                break;
            }
            Vertex s, t;
            std::cout << "Введите начальную вершину: ";
            std::cin >> s;
            std::cout << "Введите конечную вершину (-1, если не нужно восстанавливать путь): ";
            std::cin >> t;

            auto start = std::chrono::steady_clock::now();
            auto res = bellman_ford(current_graph, s);
            auto finish = std::chrono::steady_clock::now();
            double time_ms =
                std::chrono::duration_cast<std::chrono::microseconds>(finish - start)
                    .count() /
                1000.0;

            if (res.has_negative_cycle) {
                std::cout << "Обнаружен цикл отрицательного веса. Результаты могут быть некорректны.\n";
            }

            std::cout << "Алгоритм Беллмана–Форда завершён за " << time_ms << " мс.\n";
            print_distances(res);

            if (t >= 0) {
                auto path = restore_path(s, t, res.parent);
                std::cout << "Путь от " << s << " до " << t << ": ";
                print_path(path);
            }
            break;
        }
        case 7: {
            if (!has_graph) {
                std::cout << "Сначала создайте или загрузите граф.\n";
                break;
            }
            Vertex s;
            std::cout << "Введите начальную вершину: ";
            std::cin >> s;

            last_benchmarks = compare_algorithms_on_graph(current_graph, s);
            std::cout << "Сравнение выполнено.\n";
            for (int i = 0; i < last_benchmarks.GetSize(); ++i) {
                BenchmarkRecord r = last_benchmarks.Get(i);
                std::cout << "Алгоритм: " << r.algorithm
                          << ", время: " << r.time_ms << " мс"
                          << ", корректность: " << (r.ok ? "OK" : "FAIL") << "\n";
            }
            break;
        }
        case 8: {
            if (last_benchmarks.GetSize() == 0) {
                std::cout << "Нет данных для сохранения. Сначала выполните сравнение.\n";
                break;
            }
            std::string filename;
            std::cout << "Введите имя CSV-файла: ";
            std::cin >> filename;
            if (save_benchmarks_to_csv(last_benchmarks, filename)) {
                std::cout << "Данные успешно сохранены в " << filename << "\n";
            } else {
                std::cout << "Не удалось сохранить данные.\n";
            }
            break;
        }
        case 9: {
            run_all_tests();
            break;
        }
        case 10: {
            run_tictactoe_advisor();
            break;
        }
        case 11: {
            run_tictactoe_pygame();
            break;
        }
        case 12: {
            if (!has_graph) {
                std::cout << "Сначала создайте или загрузите граф.\n";
                break;
            }
            std::string filename;
            std::cout << "Введите имя файла для сохранения графа (например, graph.txt): ";
            std::cin >> filename;
            if (!save_graph_to_file(current_graph, filename)) {
                std::cout << "Не удалось сохранить граф в файл.\n";
                break;
            }
            std::cout << "Граф сохранён в " << filename << ". Откроется окно Python-визуализации.\n";

            std::string cmd = "python3 visualize_vectors.py \"" + filename + "\"";
            int res = std::system(cmd.c_str());
            if (res != 0) {
                std::cout << "Не удалось запустить visualize_vectors.py.\n"
                          << "Убедитесь, что установлены Python 3 и библиотека matplotlib,\n"
                          << "а также что скрипт visualize_vectors.py находится в том же каталоге,\n"
                          << "откуда запускается исполняемый файл.\n";
            }
            break;
        }
        default:
            std::cout << "Неизвестный пункт меню.\n";
            break;
        }
    }

    return 0;
}


