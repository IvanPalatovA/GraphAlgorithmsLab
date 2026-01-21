#pragma once

#include <iostream>
#include <limits>
#include <chrono>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

// Наши структуры данных
#include "../LibrarySequence/LinkedList.h"
#include "../LibrarySequence/DynamicArray.h"
#include "../Realization/IPriorityQueue/hash_priority_queue.h"

// ------------------------------------------------------------
// Общие типы
// ------------------------------------------------------------

using Vertex = int;
using Weight = double;
constexpr Weight INF = std::numeric_limits<Weight>::infinity();

// ------------------------------------------------------------
// Структура данных: ориентированный/неориентированный граф
// ------------------------------------------------------------

struct Edge {
    Vertex to;
    Weight weight;
};

// Граф на основе массива списков смежности (LinkedList из LibrarySequence),
// без использования std::vector.
class Graph {
public:
    Graph(int n = 0, bool directed = true)
        : n_(n), directed_(directed) {
        if (n_ > 0) {
            adj_ = new LinkedList<Edge>[n_];
        } else {
            adj_ = nullptr;
        }
    }

    Graph(const Graph& other)
        : n_(other.n_), directed_(other.directed_) {
        if (n_ > 0) {
            adj_ = new LinkedList<Edge>[n_];
            for (int i = 0; i < n_; ++i) {
                adj_[i] = other.adj_[i];
            }
        } else {
            adj_ = nullptr;
        }
    }

    Graph& operator=(const Graph& other) {
        if (this == &other) return *this;
        delete[] adj_;
        n_ = other.n_;
        directed_ = other.directed_;
        if (n_ > 0) {
            adj_ = new LinkedList<Edge>[n_];
            for (int i = 0; i < n_; ++i) {
                adj_[i] = other.adj_[i];
            }
        } else {
            adj_ = nullptr;
        }
        return *this;
    }

    Graph(Graph&& other) noexcept
        : n_(other.n_), directed_(other.directed_), adj_(other.adj_) {
        other.n_ = 0;
        other.adj_ = nullptr;
    }

    Graph& operator=(Graph&& other) noexcept {
        if (this == &other) return *this;
        delete[] adj_;
        n_ = other.n_;
        directed_ = other.directed_;
        adj_ = other.adj_;
        other.n_ = 0;
        other.adj_ = nullptr;
        return *this;
    }

    ~Graph() {
        delete[] adj_;
    }

    int vertex_count() const { return n_; }

    bool is_directed() const { return directed_; }

    void resize(int n) {
        if (n == n_) return;
        LinkedList<Edge>* new_adj = nullptr;
        if (n > 0) {
            new_adj = new LinkedList<Edge>[n];
            int copy_n = (n < n_) ? n : n_;
            for (int i = 0; i < copy_n; ++i) {
                new_adj[i] = adj_[i];
            }
        }
        delete[] adj_;
        adj_ = new_adj;
        n_ = n;
    }

    void add_edge(Vertex u, Vertex v, Weight w) {
        if (u < 0 || v < 0 || u >= vertex_count() || v >= vertex_count()) {
            throw std::out_of_range("Некорректные номера вершин");
        }
        adj_[u].Append(Edge{v, w});
        if (!directed_ && u != v) {
            adj_[v].Append(Edge{u, w});
        }
    }

    const LinkedList<Edge>& neighbors(Vertex u) const {
        return adj_[u];
    }

    std::size_t edge_count() const {
        std::size_t m = 0;
        for (int i = 0; i < n_; ++i) {
            m += static_cast<std::size_t>(adj_[i].GetLength());
        }
        if (!directed_) {
            m /= 2;
        }
        return m;
    }

private:
    int n_{0};
    bool directed_{true};
    LinkedList<Edge>* adj_{nullptr};
};

// ------------------------------------------------------------
// Алгоритмы поиска кратчайших путей
// ------------------------------------------------------------

struct ShortestPathResult {
    DynamicArray<Weight> dist;
    DynamicArray<Vertex> parent;
    bool has_negative_cycle{false};

    ShortestPathResult() : dist(), parent(), has_negative_cycle(false) {}
    explicit ShortestPathResult(int n)
        : dist(n), parent(n), has_negative_cycle(false) {}
};

inline ShortestPathResult dijkstra(const Graph& g, Vertex source) {
    const int n = g.vertex_count();
    ShortestPathResult res(n);
    for (int i = 0; i < n; ++i) {
        res.dist.Set(i, INF);
        res.parent.Set(i, -1);
    }

    if (source < 0 || source >= n) {
        return res;
    }

    // Очередь с приоритетами из готовой реализации
    HashTablePriorityQueue<Vertex, Weight> pq;

    res.dist.Set(source, 0.0);
    pq.Enqueue(source, 0.0);

    bool* used = nullptr;
    if (n > 0) {
        used = new bool[n];
        for (int i = 0; i < n; ++i) used[i] = false;
    }

    while (!pq.IsEmpty()) {
        Vertex u = pq.Dequeue();
        if (u < 0 || u >= n) continue;
        if (used[u]) continue;
        used[u] = true;

        const LinkedList<Edge>& neigh = g.neighbors(u);
        auto it = neigh.GetEnumerator();
        // Идём по всем рёбрам, исходящим из u
        while (it->MoveNext()) {
            Edge e = it->Current();
            if (e.weight < 0) {
                // Для отрицательных весов Дейкстра неприменима
                continue;
            }
            // текущее известное расстояние от источника до u.
            Weight du = res.dist.Get(u);
            if (du == INF) continue;
            // расстояние от источника до вершины v, которую мы только что извлекли из очереди.
            Weight nd = du + e.weight;
            // текущее известное расстояние от источника до вершины v.
            Weight cur = res.dist.Get(e.to);
            // если найденное расстояние меньше текущего, то обновляем расстояние и родителя вершины v
            if (nd < cur) {
                res.dist.Set(e.to, nd);
                res.parent.Set(e.to, u);
                pq.Enqueue(e.to, nd);
            }
        }
        delete it;
    }

    delete[] used;
    return res;
}

inline ShortestPathResult bellman_ford(const Graph& g, Vertex source) {
    const int n = g.vertex_count();
    ShortestPathResult res(n);
    for (int i = 0; i < n; ++i) {
        res.dist.Set(i, INF);
        res.parent.Set(i, -1);
    }
    res.has_negative_cycle = false;

    if (source < 0 || source >= n) {
        return res;
    }

    res.dist.Set(source, 0.0);

    struct BFEdge { Vertex u, v; Weight w; };
    std::size_t m = g.edge_count();
    DynamicArray<BFEdge> edges(static_cast<int>(m));
    int idx = 0;
    for (Vertex u = 0; u < n; ++u) {
        const LinkedList<Edge>& neigh = g.neighbors(u);
        auto it = neigh.GetEnumerator();
        while (it->MoveNext()) {
            Edge e = it->Current();
            if (idx < static_cast<int>(m)) {
                edges.Set(idx, BFEdge{u, e.to, e.weight});
                ++idx;
            }
        }
        delete it;
    }

    for (int i = 0; i < n - 1; ++i) {
        bool changed = false;
        for (int j = 0; j < idx; ++j) {
            BFEdge e = edges.Get(j);
            Weight du = res.dist.Get(e.u);
            if (du != INF && du + e.w < res.dist.Get(e.v)) {
                res.dist.Set(e.v, du + e.w);
                res.parent.Set(e.v, e.u);
                changed = true;
            }
        }
        // Если за полный проход по рёбрам ни одно расстояние не изменилось,
        // значит все кратчайшие пути уже найдены раньше, и можно выйти раньше, чем через n-1 итераций (оптимизация).
        if (!changed) break;
    }
    // если и после всех итераций можно ещё улучшить расстояние до e.v, значит есть отрицательный цикл.
    for (int j = 0; j < idx; ++j) {
        BFEdge e = edges.Get(j);
        Weight du = res.dist.Get(e.u);
        if (du != INF && du + e.w < res.dist.Get(e.v)) {
            res.has_negative_cycle = true;
            break;
        }
    }

    return res;
}

// ------------------------------------------------------------
// Генерация графов
// ------------------------------------------------------------

inline Graph generate_random_graph(int n, double edge_probability, Weight min_w,
                                   Weight max_w, bool directed) {
    if (n < 0) throw std::invalid_argument("Число вершин не может быть отрицательным");
    if (edge_probability < 0.0 || edge_probability > 1.0) {
        throw std::invalid_argument("Вероятность ребра должна быть в [0, 1]");
    }
    if (min_w > max_w) std::swap(min_w, max_w);

    Graph g(n, directed);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> prob_dist(0.0, 1.0);
    std::uniform_real_distribution<Weight> w_dist(min_w, max_w);

    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (!directed && v <= u) continue;
            if (u == v) continue;
            if (prob_dist(gen) <= edge_probability) {
                g.add_edge(u, v, w_dist(gen));
            }
        }
    }

    return g;
}

// ------------------------------------------------------------
// Вспомогательные функции
// ------------------------------------------------------------

inline DynamicArray<Vertex> restore_path(Vertex source, Vertex target,
                                         const DynamicArray<Vertex>& parent) {
    int n = parent.GetSize();
    DynamicArray<Vertex> path;
    if (target < 0 || target >= n) {
        return path;
    }
    if (parent.Get(target) == -1 && target != source) {
        return path;
    }

    DynamicArray<Vertex> rev(n);
    int len = 0;
    for (Vertex v = target; v != -1; v = parent.Get(v)) {
        rev.Set(len, v);
        ++len;
        if (v == source) break;
    }
    if (len == 0 || rev.Get(len - 1) != source) {
        return DynamicArray<Vertex>();
    }

    path = DynamicArray<Vertex>(len);
    for (int i = 0; i < len; ++i) {
        path.Set(i, rev.Get(len - 1 - i));
    }
    return path;
}

inline void print_path(const DynamicArray<Vertex>& path) {
    int len = path.GetSize();
    if (len == 0) {
        std::cout << "Путь не найден." << std::endl;
        return;
    }
    for (int i = 0; i < len; ++i) {
        std::cout << path.Get(i);
        if (i + 1 < len) {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;
}

inline void print_distances(const ShortestPathResult& res) {
    std::cout << "Вершина : расстояние" << std::endl;
    int n = res.dist.GetSize();
    for (int i = 0; i < n; ++i) {
        std::cout << i << " : ";
        if (res.dist.Get(i) == INF) {
            std::cout << "INF";
        } else {
            std::cout << res.dist.Get(i);
        }
        std::cout << '\n';
    }
}

inline void print_graph(const Graph& g) {
    std::cout << (g.is_directed() ? "Ориентированный" : "Неориентированный")
              << " граф, вершин: " << g.vertex_count()
              << ", рёбер: " << g.edge_count() << std::endl;
    for (Vertex u = 0; u < g.vertex_count(); ++u) {
        std::cout << u << ": ";
        const LinkedList<Edge>& neigh = g.neighbors(u);
        auto it = neigh.GetEnumerator();
        while (it->MoveNext()) {
            Edge e = it->Current();
            std::cout << "(" << e.to << ", w=" << e.weight << ") ";
        }
        delete it;
        std::cout << '\n';
    }
}

// ------------------------------------------------------------
// Загрузка/сохранение графа и бенчмарки
// ------------------------------------------------------------

inline bool save_graph_to_file(const Graph& g, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) return false;

    out << g.vertex_count() << " " << g.edge_count() << " " << (g.is_directed() ? 1 : 0)
        << "\n";

    for (Vertex u = 0; u < g.vertex_count(); ++u) {
        const LinkedList<Edge>& neigh = g.neighbors(u);
        auto it = neigh.GetEnumerator();
        while (it->MoveNext()) {
            Edge e = it->Current();
            if (!g.is_directed() && u > e.to) {
                continue;
            }
            out << u << " " << e.to << " " << e.weight << "\n";
        }
        delete it;
    }

    return true;
}

inline bool load_graph_from_file(Graph& g, const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return false;

    int n;
    std::size_t m;
    int directed_flag;
    if (!(in >> n >> m >> directed_flag)) {
        return false;
    }
    Graph tmp(n, directed_flag != 0);
    for (std::size_t i = 0; i < m; ++i) {
        Vertex u, v;
        Weight w;
        if (!(in >> u >> v >> w)) {
            return false;
        }
        tmp.add_edge(u, v, w);
    }

    g = std::move(tmp);
    return true;
}

struct BenchmarkRecord {
    int vertices;
    std::size_t edges;
    std::string algorithm;
    double time_ms;
    bool ok;
};

inline DynamicArray<BenchmarkRecord> compare_algorithms_on_graph(
    const Graph& g, Vertex source) {
    DynamicArray<BenchmarkRecord> records(2);

    auto t1 = std::chrono::steady_clock::now();
    auto res_d = dijkstra(g, source);
    auto t2 = std::chrono::steady_clock::now();
    double time_d_ms =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() /
        1000.0;

    auto t3 = std::chrono::steady_clock::now();
    auto res_b = bellman_ford(g, source);
    auto t4 = std::chrono::steady_clock::now();
    double time_b_ms =
        std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count() /
        1000.0;

    bool same = true;
    if (res_d.dist.GetSize() == res_b.dist.GetSize()) {
        int n = res_d.dist.GetSize();
        for (int i = 0; i < n; ++i) {
            bool inf1 = (res_d.dist.Get(i) == INF);
            bool inf2 = (res_b.dist.Get(i) == INF);
            if (inf1 != inf2) {
                same = false;
                break;
            }
            if (!inf1 && std::abs(res_d.dist.Get(i) - res_b.dist.Get(i)) > 1e-6) {
                same = false;
                break;
            }
        }
    } else {
        same = false;
    }

    records.Set(0, BenchmarkRecord{
        g.vertex_count(),
        g.edge_count(),
        "Dijkstra",
        time_d_ms,
        res_d.dist.GetSize() > 0});

    records.Set(1, BenchmarkRecord{
        g.vertex_count(),
        g.edge_count(),
        "Bellman-Ford",
        time_b_ms,
        !res_b.has_negative_cycle && same});

    return records;
}

inline bool save_benchmarks_to_csv(const DynamicArray<BenchmarkRecord>& records,
                                   const std::string& filename) {
    std::ofstream out(filename);
    if (!out) return false;

    out << "vertices,edges,algorithm,time_ms,ok\n";
    int n = records.GetSize();
    for (int i = 0; i < n; ++i) {
        BenchmarkRecord r = records.Get(i);
        out << r.vertices << ","
            << r.edges << ","
            << r.algorithm << ","
            << std::fixed << std::setprecision(3) << r.time_ms << ","
            << (r.ok ? 1 : 0) << "\n";
    }
    return true;
}

// ------------------------------------------------------------
// Тесты
// ------------------------------------------------------------

inline bool run_shortest_path_tests() {
    bool all_ok = true;

    {
        Graph g(5, true);
        g.add_edge(0, 1, 2.0);
        g.add_edge(0, 2, 5.0);
        g.add_edge(1, 2, 1.0);
        g.add_edge(1, 3, 2.0);
        g.add_edge(2, 3, 1.0);
        g.add_edge(3, 4, 3.0);

        auto dres = dijkstra(g, 0);
        auto bres = bellman_ford(g, 0);

        Weight expected_raw[5] = {0.0, 2.0, 3.0, 4.0, 7.0};
        DynamicArray<Weight> expected(5);
        for (int i = 0; i < 5; ++i) expected.Set(i, expected_raw[i]);

        for (int i = 0; i < 5; ++i) {
            if (std::abs(dres.dist.Get(i) - expected.Get(i)) > 1e-6) {
                std::cout << "Тест Дейкстры не пройден для вершины " << i << '\n';
                all_ok = false;
            }
            if (std::abs(bres.dist.Get(i) - expected.Get(i)) > 1e-6) {
                std::cout << "Тест Беллмана–Форда не пройден для вершины " << i << '\n';
                all_ok = false;
            }
        }
    }

    {
        Graph g(3, true);
        g.add_edge(0, 1, 1.0);
        g.add_edge(1, 2, -2.0);
        g.add_edge(0, 2, 4.0);

        auto bres = bellman_ford(g, 0);
        if (bres.has_negative_cycle) {
            std::cout << "Ошибочное обнаружение отрицательного цикла\n";
            all_ok = false;
        }
        if (std::abs(bres.dist.Get(2) - (-1.0)) > 1e-6) {
            std::cout << "Тест Беллмана–Форда с отрицательными весами не пройден\n";
            all_ok = false;
        }
    }

    {
        Graph g(3, true);
        g.add_edge(0, 1, 1.0);
        auto dres = dijkstra(g, 0);
        auto bres = bellman_ford(g, 0);
        if (dres.dist.Get(2) != INF || bres.dist.Get(2) != INF) {
            std::cout << "Тест на недостижимую вершину не пройден\n";
            all_ok = false;
        }
    }

    return all_ok;
}

inline bool run_generation_tests() {
    bool all_ok = true;

    {
        int n = 10;
        double p = 0.0;
        auto g = generate_random_graph(n, p, 1.0, 10.0, true);
        if (g.edge_count() != 0) {
            std::cout << "Генерация с p=0 дала ненулевое число рёбер\n";
            all_ok = false;
        }
    }

    {
        int n = 10;
        double p = 1.0;
        auto g = generate_random_graph(n, p, 1.0, 10.0, false);
        std::size_t expected = static_cast<std::size_t>(n) * (n - 1) / 2;
        if (g.edge_count() != expected) {
            std::cout << "Генерация полного неориентированного графа дала "
                      << g.edge_count() << " рёбер, ожидалось " << expected << "\n";
            all_ok = false;
        }
    }

    return all_ok;
}

inline void run_all_tests() {
    bool ok_sp = run_shortest_path_tests();
    bool ok_gen = run_generation_tests();

    std::cout << "Результаты тестов:" << std::endl;
    std::cout << "  Кратчайшие пути: " << (ok_sp ? "OK" : "FAIL") << std::endl;
    std::cout << "  Генерация графов: " << (ok_gen ? "OK" : "FAIL") << std::endl;
}


