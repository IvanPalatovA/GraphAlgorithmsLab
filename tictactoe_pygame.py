import sys
import random
from typing import List, Optional, Tuple

import pygame


# ------------------------------------------------------------
# Настройки отрисовки
# ------------------------------------------------------------

WINDOW_SIZE = 600  # базовый размер окна по меньшей стороне
LINE_COLOR = (0, 0, 0)
BG_COLOR = (240, 240, 240)
X_COLOR = (200, 0, 0)
O_COLOR = (0, 0, 200)
WIN_LINE_COLOR = (0, 180, 0)


class GameState:
    """Модель игры крестики‑нолики произвольного размера.

    size       — размер поля (n x n)
    win_length — сколько фигур подряд нужно для победы (k, 1 <= k <= n)
    """

    def __init__(
        self,
        size: int,
        win_length: int,
        human_plays_x: bool,
        human_starts: bool,
    ):
        self.size = size
        self.win_length = win_length
        self.board: List[List[str]] = [["." for _ in range(size)] for _ in range(size)]
        self.human_plays_x = human_plays_x
        self.current_player_x = human_starts if human_plays_x else not human_starts
        # флаг, чей сейчас ход человек (True) или компьютер (False)
        self.human_turn = human_starts
        self.winner: Optional[str] = None  # 'X', 'O' или 'D' (ничья)
        self.win_line: Optional[Tuple[Tuple[int, int], Tuple[int, int]]] = None

    def cell(self, r: int, c: int) -> str:
        return self.board[r][c]

    def make_move(self, r: int, c: int) -> bool:
        """Сделать ход в клетку (r, c). Возвращает True, если ход удался."""
        if self.winner is not None:
            return False
        if not (0 <= r < self.size and 0 <= c < self.size):
            return False
        if self.board[r][c] != ".":
            return False

        symbol = "X" if self.current_player_x else "O"
        self.board[r][c] = symbol
        self._update_winner_after_move(r, c, symbol)

        # смена игрока, если игра не закончилась
        if self.winner is None:
            self.current_player_x = not self.current_player_x
            self.human_turn = not self.human_turn
        return True

    def _update_winner_after_move(self, r: int, c: int, symbol: str) -> None:
        """Проверка победы/ничьей после хода symbol в (r, c)."""
        won, line = self._find_win_from(r, c, symbol)
        if won:
            self.winner = symbol
            self.win_line = line
            return

        # проверка ничьей (нет свободных клеток)
        n = self.size
        if all(self.board[i][j] != "." for i in range(n) for j in range(n)):
            self.winner = "D"
            self.win_line = None

    def _find_win_from(
        self, r: int, c: int, symbol: str
    ) -> Tuple[bool, Optional[Tuple[Tuple[int, int], Tuple[int, int]]]]:
        """Ищет выигрышную линию длины >= win_length, проходящую через (r, c)."""
        if self.board[r][c] != symbol:
            return False, None

        n = self.size
        k = self.win_length

        def count_in_direction(dr: int, dc: int) -> Tuple[int, Tuple[int, int], Tuple[int, int]]:
            count = 1
            start_r, start_c = r, c
            end_r, end_c = r, c

            # вперёд
            rr, cc = r + dr, c + dc
            while 0 <= rr < n and 0 <= cc < n and self.board[rr][cc] == symbol:
                count += 1
                end_r, end_c = rr, cc
                rr += dr
                cc += dc

            # назад
            rr, cc = r - dr, c - dc
            while 0 <= rr < n and 0 <= cc < n and self.board[rr][cc] == symbol:
                count += 1
                start_r, start_c = rr, cc
                rr -= dr
                cc -= dc

            return count, (start_r, start_c), (end_r, end_c)

        directions = [(0, 1), (1, 0), (1, 1), (1, -1)]

        for dr, dc in directions:
            cnt, start, end = count_in_direction(dr, dc)
            if cnt >= k:
                return True, (start, end)

        return False, None

    def available_moves(self) -> List[Tuple[int, int]]:
        return [(r, c) for r in range(self.size)
                for c in range(self.size)
                if self.board[r][c] == "."]


def ai_choose_move(game: GameState) -> Optional[Tuple[int, int]]:
    """
    Простейший ИИ:
    1. Если есть выигрышный ход — делает его.
    2. Если есть ход, который блокирует немедленную победу соперника — блокирует.
    3. Иначе — случайный свободный ход.
    """
    moves = game.available_moves()
    if not moves:
        return None

    ai_symbol = "X" if game.current_player_x else "O"
    human_symbol = "O" if ai_symbol == "X" else "X"

    def is_winning_move(r: int, c: int, symbol: str) -> bool:
        # временно ставим символ и используем общую логику поиска k-в-ряд
        game.board[r][c] = symbol
        win, _ = game._find_win_from(r, c, symbol)
        game.board[r][c] = "."
        return win

    # 1. Попробовать выиграть
    for r, c in moves:
        if is_winning_move(r, c, ai_symbol):
            return r, c

    # 2. Попробовать заблокировать соперника
    for r, c in moves:
        if is_winning_move(r, c, human_symbol):
            return r, c

    # 3. Иначе — случайный ход
    return random.choice(moves)


def draw_board(
    screen: pygame.Surface,
    game: GameState,
    font: pygame.font.Font,
    allow_restart_hint: bool = True,
) -> None:
    screen.fill(BG_COLOR)
    width, height = screen.get_size()
    n = game.size

    # вычисляем размер клетки
    cell_size = min(width, height) // n
    offset_x = (width - cell_size * n) // 2
    offset_y = (height - cell_size * n) // 2

    # линии сетки
    for i in range(n + 1):
        # вертикальные
        x = offset_x + i * cell_size
        pygame.draw.line(screen, LINE_COLOR, (x, offset_y), (x, offset_y + n * cell_size), 2)
        # горизонтальные
        y = offset_y + i * cell_size
        pygame.draw.line(screen, LINE_COLOR, (offset_x, y), (offset_x + n * cell_size, y), 2)

    # фигуры
    for r in range(n):
        for c in range(n):
            val = game.cell(r, c)
            if val == ".":
                continue
            center_x = offset_x + c * cell_size + cell_size // 2
            center_y = offset_y + r * cell_size + cell_size // 2
            if val == "X":
                draw_x(screen, center_x, center_y, cell_size // 2 - 10)
            elif val == "O":
                draw_o(screen, center_x, center_y, cell_size // 2 - 10)

    # линия выигрыша
    if game.win_line is not None:
        (r1, c1), (r2, c2) = game.win_line
        x1 = offset_x + c1 * cell_size + cell_size // 2
        y1 = offset_y + r1 * cell_size + cell_size // 2
        x2 = offset_x + c2 * cell_size + cell_size // 2
        y2 = offset_y + r2 * cell_size + cell_size // 2
        pygame.draw.line(screen, WIN_LINE_COLOR, (x1, y1), (x2, y2), 6)

    # сообщение о статусе
    message = ""
    if game.winner is None:
        current = "X" if game.current_player_x else "O"
        who = "Вы" if game.human_turn else "Компьютер"
        message = f"Ход {who} ({current})"
    else:
        base = ""
        if game.winner == "D":
            base = "Ничья."
        elif (game.winner == "X" and game.human_plays_x) or (
            game.winner == "O" and not game.human_plays_x
        ):
            base = f"Вы победили ({game.winner})!"
        else:
            base = f"Компьютер победил ({game.winner})."

        if allow_restart_hint:
            message = base + " Нажмите R — новая игра, Esc — выход."
        else:
            message = base

    text_surface = font.render(message, True, (0, 0, 0))
    screen.blit(text_surface, (20, 10))


def draw_x(screen: pygame.Surface, cx: int, cy: int, radius: int) -> None:
    dx = dy = radius
    pygame.draw.line(screen, X_COLOR, (cx - dx, cy - dy), (cx + dx, cy + dy), 4)
    pygame.draw.line(screen, X_COLOR, (cx - dx, cy + dy), (cx + dx, cy - dy), 4)


def draw_o(screen: pygame.Surface, cx: int, cy: int, radius: int) -> None:
    pygame.draw.circle(screen, O_COLOR, (cx, cy), radius, 4)


def run_pygame_game(size: int, win_length: int, human_starts: bool) -> None:
    """
    Запуск игры в отдельном окне pygame.
    Пользователь управляет мышью, компьютер ходит автоматически.
    По условию лабораторной считаем, что человек играет крестиками (X),
    компьютер — ноликами (O); при необходимости это легко расширить.
    """
    pygame.init()
    pygame.display.set_caption("Крестики-нолики (pygame)")
    screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE))
    clock = pygame.time.Clock()
    font = pygame.font.SysFont(None, 28)

    # внешний цикл — позволяет перезапускать игру без выхода из приложения
    running = True
    # фиксируем, кто ходит первым, для всех перезапусков
    initial_human_starts = human_starts

    while running:
        # создаём новое состояние игры
        game = GameState(
            size=size,
            win_length=win_length,
            human_plays_x=True,
            human_starts=initial_human_starts,
        )

        round_active = True
        while round_active and running:
            # ход компьютера
            if game.winner is None and not game.human_turn:
                move = ai_choose_move(game)
                if move is not None:
                    game.make_move(*move)

            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                    round_active = False
                elif event.type == pygame.KEYDOWN:
                    if game.winner is not None:
                        if event.key == pygame.K_r:
                            # выйти из текущего раунда, чтобы начать новый
                            round_active = False
                        elif event.key in (pygame.K_ESCAPE, pygame.K_q):
                            running = False
                            round_active = False
                    elif event.key in (pygame.K_ESCAPE, pygame.K_q):
                        # выход в любой момент по Esc / Q
                        running = False
                        round_active = False
                elif (
                    event.type == pygame.MOUSEBUTTONDOWN
                    and event.button == 1
                    and game.winner is None
                    and game.human_turn
                ):
                    handle_mouse_click(game, event.pos, screen.get_size())

            draw_board(screen, game, font)
            pygame.display.flip()
            clock.tick(60)

    pygame.quit()


def handle_mouse_click(game: GameState, pos: Tuple[int, int], window_size: Tuple[int, int]) -> None:
    width, height = window_size
    n = game.size
    cell_size = min(width, height) // n
    offset_x = (width - cell_size * n) // 2
    offset_y = (height - cell_size * n) // 2

    x, y = pos
    if not (offset_x <= x < offset_x + cell_size * n and
            offset_y <= y < offset_y + cell_size * n):
        return

    c = (x - offset_x) // cell_size
    r = (y - offset_y) // cell_size
    game.make_move(r, c)


def ask_game_parameters() -> Optional[Tuple[int, int, bool]]:
    """
    Запрос параметров игры из консоли:
    - размер поля n
    - длина победной линии k (сколько в ряд нужно для победы)
    - кто ходит первым (1 – человек, 2 – компьютер)
    Возвращает (n, k, human_starts) или None при ошибке/отмене.
    """
    try:
        print("Игра 'Крестики‑нолики' с графическим интерфейсом (pygame).")
        n = int(input("Введите размер поля (например, 3): ").strip())
        if n <= 0:
            print("Размер поля должен быть положительным числом.")
            return None

        k_str = input(
            f"Сколько в ряд нужно для победы (1..{n}, Enter — равно размеру поля): "
        ).strip()
        if k_str == "":
            k = n
        else:
            k = int(k_str)
            if k <= 0 or k > n:
                print(f"Число в ряд должно быть в диапазоне 1..{n}.")
                return None

        print("Кто ходит первым?")
        print("1 — человек (X)")
        print("2 — компьютер (O)")
        first = input("Ваш выбор (1/2): ").strip()
        if first not in ("1", "2"):
            print("Некорректный выбор.")
            return None

        human_starts = first == "1"
        return n, k, human_starts
    except (EOFError, KeyboardInterrupt):
        print("\nВвод прерван.")
        return None
    except ValueError:
        print("Ожидалось целое число.")
        return None


def main() -> None:
    """
    Точка входа.
    Пользователь сначала в консоли выбирает режим "игра в крестики‑нолики",
    размер поля и кто ходит первым, затем открывается окно pygame.
    """
    # Если скрипт запускается напрямую, спрашиваем параметры
    params = ask_game_parameters()
    if params is None:
        return

    size, win_length, human_starts = params
    run_pygame_game(size=size, win_length=win_length, human_starts=human_starts)


if __name__ == "__main__":
    main()


