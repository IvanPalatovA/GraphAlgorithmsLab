#pragma once

#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>

#include "../LibrarySequence/DynamicArray.h"

// ------------------------------------------------------------
// Крестики‑нолики как граф состояний
// ------------------------------------------------------------
//
// Вершина графа  — игровая позиция (TTTBoard),
// дуга            — ход (добавление X или O в свободную клетку).
// По этому ориентированному графу выполняется поиск
// лучшего продолжения (минимакс с отсечениями).
// ------------------------------------------------------------

struct TTTBoard {
    int size;
    DynamicArray<char> cells; // '.', 'X', 'O'

    explicit TTTBoard(int n = 3)
        : size(n), cells(n * n) {
        for (int i = 0; i < n * n; ++i) {
            cells.Set(i, '.');
        }
    }

    char get(int r, int c) const {
        return cells.Get(r * size + c);
    }

    void set(int r, int c, char v) {
        cells.Set(r * size + c, v);
    }
};

inline bool ttt_check_win(const TTTBoard& b, char p) {
    const int n = b.size;
    // строки
    for (int r = 0; r < n; ++r) {
        bool ok = true;
        for (int c = 0; c < n; ++c) {
            if (b.get(r, c) != p) { ok = false; break; }
        }
        if (ok) return true;
    }
    // столбцы
    for (int c = 0; c < n; ++c) {
        bool ok = true;
        for (int r = 0; r < n; ++r) {
            if (b.get(r, c) != p) { ok = false; break; }
        }
        if (ok) return true;
    }
    // главная диагональ
    {
        bool ok = true;
        for (int i = 0; i < n; ++i) {
            if (b.get(i, i) != p) { ok = false; break; }
        }
        if (ok) return true;
    }
    // побочная диагональ
    {
        bool ok = true;
        for (int i = 0; i < n; ++i) {
            if (b.get(i, n - 1 - i) != p) { ok = false; break; }
        }
        if (ok) return true;
    }
    return false;
}

inline bool ttt_has_empty(const TTTBoard& b) {
    const int n2 = b.size * b.size;
    for (int i = 0; i < n2; ++i) {
        if (b.cells.Get(i) == '.') return true;
    }
    return false;
}

// Статическая оценка позиции:
//  +10  — выигрыш O (ноликов),
//  -10  — выигрыш X (крестиков),
//   0   — никто не выиграл (пока или ничья).
inline int ttt_static_eval(const TTTBoard& b) {
    if (ttt_check_win(b, 'O')) return 10;
    if (ttt_check_win(b, 'X')) return -10;
    return 0;
}

// Вспомогательная функция: границы «интересных» ходов.
// Если поле большое, допускаем ходы только в окрестности уже занятых
// клеток (рамка +1), чтобы отсечь заведомо эквивалентные дальние ходы.
inline void ttt_compute_active_box(const TTTBoard& b,
                                   int& r_min, int& r_max,
                                   int& c_min, int& c_max) {
    const int n = b.size;
    bool any = false;
    r_min = n; r_max = -1; c_min = n; c_max = -1;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            char v = b.get(r, c);
            if (v == '.' || v == ' ') continue;
            if (!any) {
                any = true;
                r_min = r_max = r;
                c_min = c_max = c;
            } else {
                if (r < r_min) r_min = r;
                if (r > r_max) r_max = r;
                if (c < c_min) c_min = c;
                if (c > c_max) c_max = c;
            }
        }
    }
    if (!any) {
        // поле пустое — разрешаем все клетки
        r_min = 0; r_max = n - 1;
        c_min = 0; c_max = n - 1;
    } else {
        if (r_min > 0) --r_min;
        if (c_min > 0) --c_min;
        if (r_max + 1 < n) ++r_max;
        if (c_max + 1 < n) ++c_max;
    }
}

// Минимакс по графу позиций.
inline int ttt_minimax(TTTBoard& b, bool o_turn, int depth, int max_depth) {
    int score = ttt_static_eval(b);
    if (score == 10 || score == -10) {
        // Для выигрыша O: 10 - depth — чем раньше выиграли, тем лучше (чуть больше число).
        // Для выигрыша X: -10 + depth — чем раньше проиграли, тем хуже (чуть меньше число).
        // чем раньше победа/поражение, тем лучше/хуже
        return score > 0 ? score - depth : score + depth;
    }
    if (!ttt_has_empty(b) || depth >= max_depth) {
        // позиция не оценена как выигрыш/проигрыш, считаем её нейтральной).
        return 0;
    }

    const int n = b.size;
    int r_min, r_max, c_min, c_max;
    ttt_compute_active_box(b, r_min, r_max, c_min, c_max);

    // Ветка, когда ход ноликов (O) — это максимизирующий игрок:
    if (o_turn) {
        int best = std::numeric_limits<int>::min();
        for (int r = r_min; r <= r_max; ++r) {
            for (int c = c_min; c <= c_max; ++c) {
                if (b.get(r, c) != '.') continue;
                b.set(r, c, 'O');
                int val = ttt_minimax(b, false, depth + 1, max_depth);
                b.set(r, c, '.');
                if (val > best) best = val;
            }
        }
        return best;
    } 
    // ветка, когда ход крестиков (X) — это минимизирующий игрок:
    else {
        int best = std::numeric_limits<int>::max();
        for (int r = r_min; r <= r_max; ++r) {
            for (int c = c_min; c <= c_max; ++c) {
                if (b.get(r, c) != '.') continue;
                b.set(r, c, 'X');
                int val = ttt_minimax(b, true, depth + 1, max_depth);
                b.set(r, c, '.');
                if (val < best) best = val;
            }
        }
        return best;
    }
}

// Поиск лучшего хода ноликов. Возвращает true, если ход найден.
inline bool ttt_recommend_move_O(const TTTBoard& b, int& best_r, int& best_c) {
    TTTBoard work = b;
    const int n = work.size;
    int r_min, r_max, c_min, c_max;
    ttt_compute_active_box(work, r_min, r_max, c_min, c_max);

    bool found = false;
    int best_val = std::numeric_limits<int>::min();

    int empty_cnt = 0;
    for (int i = 0; i < n * n; ++i) {
        if (work.cells.Get(i) == '.') ++empty_cnt;
    }
    int max_depth = empty_cnt; // для 3x3 можно просматривать до конца

    for (int r = r_min; r <= r_max; ++r) {
        for (int c = c_min; c <= c_max; ++c) {
            if (work.get(r, c) != '.') continue;
            work.set(r, c, 'O');
            int val = ttt_minimax(work, false, 1, max_depth);
            work.set(r, c, '.');
            if (!found || val > best_val) {
                found = true;
                best_val = val;
                best_r = r;
                best_c = c;
            }
        }
    }
    return found;
}

// ------------------------------------------------------------
// Консольный интерфейс
// ------------------------------------------------------------

inline void ttt_print_board(const TTTBoard& b) {
    for (int r = 0; r < b.size; ++r) {
        for (int c = 0; c < b.size; ++c) {
            std::cout << b.get(r, c);
            if (c + 1 < b.size) std::cout << " ";
        }
        std::cout << "\n";
    }
}

inline void run_tictactoe_advisor() {
    int n;
    std::cout << "Введите размер поля (например, 3): ";
    if (!(std::cin >> n) || n <= 0) {
        std::cout << "Некорректный размер.\n";
        return;
    }

    TTTBoard board(n);
    std::cout << "Введите положение X и O на поле построчно.\n"
              << "Используйте символы: X, O, . (точка для пустой клетки).\n";
    for (int r = 0; r < n; ++r) {
        std::string row;
        std::cin >> row;
        if (static_cast<int>(row.size()) < n) {
            std::cout << "Слишком короткая строка.\n";
            return;
        }
        for (int c = 0; c < n; ++c) {
            char ch = row[c];
            if (ch != 'X' && ch != 'O' && ch != '.') {
                std::cout << "Недопустимый символ '" << ch << "' в позиции ("
                          << r << ", " << c << ").\n";
                return;
            }
            board.set(r, c, ch);
        }
    }

    std::cout << "Текущая позиция:\n";
    ttt_print_board(board);

    if (!ttt_has_empty(board)) {
        std::cout << "Свободных клеток нет, ход невозможен.\n";
        return;
    }

    int br = -1, bc = -1;
    if (!ttt_recommend_move_O(board, br, bc)) {
        std::cout << "Не удалось найти разумный ход.\n";
        return;
    }

    std::cout << "Рекомендуемый ход ноликами (O): строка " << br
              << ", столбец " << bc << "\n";

    board.set(br, bc, 'O');
    std::cout << "Позиция после хода O:\n";
    ttt_print_board(board);
}

// ------------------------------------------------------------
// Запуск графической версии игры (Python + pygame)
// ------------------------------------------------------------
//
// Предполагается, что исполняемый файл C++ запускается
// из корня проекта, где лежит скрипт tictactoe_pygame.py.
// Для работы требуется установленный Python 3 и библиотека pygame.
inline void run_tictactoe_pygame() {
    std::cout << "Запуск графической версии крестиков-ноликов (pygame)...\n";
    int result = std::system("python3 tictactoe_pygame.py");
    if (result != 0) {
        std::cout << "Не удалось запустить Python-скрипт tictactoe_pygame.py.\n"
                  << "Убедитесь, что Python 3 и pygame установлены,\n"
                  << "а также что скрипт находится в одном каталоге с исполняемым файлом\n"
                  << "или программа запущена из корня проекта.\n";
    }
}


