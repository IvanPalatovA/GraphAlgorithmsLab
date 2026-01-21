import sys
import math
from typing import List, Tuple

import matplotlib.pyplot as plt
from matplotlib.patches import FancyArrowPatch, Circle


# ------------------------------------------------------------
# Визуализация векторов (рёбер графа), сгенерированных a.out
# ------------------------------------------------------------
#
# Ожидаемый формат входного файла такой же, как у save_graph_to_file
# из C++-кода:
#   первая строка: n m directed_flag
#       n            — число вершин
#       m            — число рёбер
#       directed_flag: 1 — ориентированный граф, 0 — неориентированный
#   далее m строк: u v w
#       u, v — номера вершин (0..n-1)
#       w    — вес ребра (вещественное число)
#
# Каждое ребро (u -> v) отображается как вектор (стрелка) от точки u к точке v.
# Вершины располагаются равномерно по окружности.
#
# Использование:
#   python3 visualize_vectors.py graph.txt
# или без аргумента:
#   python3 visualize_vectors.py
#   (скрипт спросит имя файла в консоли)
#


def read_graph_from_file(filename: str) -> Tuple[int, bool, List[Tuple[int, int, float]]]:
    """Считать граф из файла формата save_graph_to_file."""
    with open(filename, "r", encoding="utf-8") as f:
        header = f.readline()
        if not header:
            raise ValueError("Файл пустой или повреждён.")

        parts = header.strip().split()
        if len(parts) != 3:
            raise ValueError("Первая строка должна быть в формате: n m directed_flag")

        n = int(parts[0])
        m = int(parts[1])
        directed_flag = int(parts[2])
        directed = directed_flag != 0

        edges: List[Tuple[int, int, float]] = []
        for line in f:
            line = line.strip()
            if not line:
                continue
            items = line.split()
            if len(items) != 3:
                continue
            u = int(items[0])
            v = int(items[1])
            w = float(items[2])
            edges.append((u, v, w))

        if m != len(edges):
            # не критично, просто предупредим
            print(
                f"Предупреждение: в заголовке указано m={m}, "
                f"но реально прочитано {len(edges)} рёбер."
            )

    return n, directed, edges


def compute_positions_on_circle(n: int, radius: float = 1.0) -> List[Tuple[float, float]]:
    """Расположить n вершин равномерно по окружности."""
    if n <= 0:
        return []
    positions: List[Tuple[float, float]] = []
    for i in range(n):
        angle = 2.0 * math.pi * i / n
        x = radius * math.cos(angle)
        y = radius * math.sin(angle)
        positions.append((x, y))
    return positions


def draw_graph_vectors(
    n: int, directed: bool, edges: List[Tuple[int, int, float]]
) -> None:
    """Отрисовать вершины и рёбра как векторы."""
    # немного увеличим радиус и потом зафиксируем одинаковые границы осей,
    # чтобы круг не "сплющивался" и не уезжал.
    radius = 3.0
    pos = compute_positions_on_circle(n, radius=radius)
    node_radius = 0.12

    fig, ax = plt.subplots(figsize=(7, 7))

    # рисуем вершины как кружки
    for i, (x, y) in enumerate(pos):
        circ = Circle(
            (x, y),
            node_radius,
            facecolor="lightblue",
            edgecolor="black",
            zorder=2,
        )
        ax.add_patch(circ)
        ax.text(
            x,
            y,
            str(i),
            ha="center",
            va="center",
            fontsize=10,
            zorder=3,
        )

    # рисуем рёбра как векторы (стрелки)
    for (u, v, w) in edges:
        if not (0 <= u < n and 0 <= v < n):
            continue

        x1, y1 = pos[u]
        x2, y2 = pos[v]

        # слегка укоротим вектор, чтобы не залезать в центр кружков
        dx = x2 - x1
        dy = y2 - y1
        length = math.hypot(dx, dy) or 1.0
        # сколько убрать с каждого конца (в тех же единицах, что и radius),
        # чтобы стрелка заканчивалась почти у границы кружков, а не вдалеке от них
        shrink = node_radius * 1.1
        if length <= 2 * shrink:
            shrink = length * 0.25
        scale = (length - 2 * shrink) / length
        sx = x1 + dx * (shrink / length)
        sy = y1 + dy * (shrink / length)
        ex = x1 + dx * scale
        ey = y1 + dy * scale

        arrowstyle = "->" if directed else "-|>"
        arrow = FancyArrowPatch(
            (sx, sy),
            (ex, ey),
            arrowstyle=arrowstyle,
            mutation_scale=15,
            linewidth=1.5,
            color="tab:gray",
            zorder=1,
        )
        ax.add_patch(arrow)

        # подпишем вес ребра чуть в стороне от середины вектора
        mx = (sx + ex) / 2.0
        my = (sy + ey) / 2.0
        # небольшой сдвиг перпендикулярно ребру, чтобы подпись не ехала по самой линии
        offset = node_radius * 1.2
        nx = -dy / length
        ny = dx / length
        label_x = mx + nx * offset
        label_y = my + ny * offset
        ax.text(
            label_x,
            label_y,
            f"{w:.2f}",
            fontsize=8,
            color="darkgreen",
            ha="center",
            va="center",
            zorder=4,
        )

    ax.set_aspect("equal", adjustable="box")
    # фиксированные границы, чтобы граф не разъезжался из‑за подписей и стрелок
    limit = radius + 0.6
    ax.set_xlim(-limit, limit)
    ax.set_ylim(-limit, limit)
    ax.axis("off")
    ax.set_title(
        f"{'Ориентированный' if directed else 'Неориентированный'} граф, "
        f"вершин: {n}, рёбер: {len(edges)}",
        fontsize=12,
    )
    plt.tight_layout()
    plt.show()


def main() -> None:
    if len(sys.argv) >= 2:
        filename = sys.argv[1]
    else:
        filename = input("Введите имя файла с графом (формат save_graph_to_file): ").strip()

    if not filename:
        print("Имя файла не задано.")
        return

    try:
        n, directed, edges = read_graph_from_file(filename)
    except Exception as e:
        print(f"Ошибка при чтении графа: {e}")
        return

    if n <= 0:
        print("Граф пустой (n <= 0), нечего отображать.")
        return

    if not edges:
        print("В файле нет рёбер, будут показаны только вершины.")

    draw_graph_vectors(n, directed, edges)


if __name__ == "__main__":
    main()


