#!/usr/bin/env python3
"""
クロスワード盤面モデル（解答支援ツール・汎用）

黒マス集合とグリッドサイズから、番号付け・タテヨコの各エントリ(語の入るマス列)を
自動導出し、答えを置いて「交差マスの文字が矛盾しないか」を検査する。

特定の問題には依存しない。黒マス配置は呼び出し側が用意して Board(black=...) に渡す
（写真からの検出は detect_grid.py を使う）。座標は (row, col)、0 始まり。
"""
from __future__ import annotations


def number_grid(black, n):
    """各白マスに番号を振り、across/down のエントリを返す。

    返り値: numbers[(r,c)] -> int, across[num] -> [cells], down[num] -> [cells]
    """
    def white(r, c):
        return 0 <= r < n and 0 <= c < n and (r, c) not in black

    numbers, across, down = {}, {}, {}
    k = 0
    for r in range(n):
        for c in range(n):
            if not white(r, c):
                continue
            starts_across = not white(r, c - 1) and white(r, c + 1)
            starts_down = not white(r - 1, c) and white(r + 1, c)
            if starts_across or starts_down:
                k += 1
                numbers[(r, c)] = k
                if starts_across:
                    cells, cc = [], c
                    while white(r, cc):
                        cells.append((r, cc)); cc += 1
                    across[k] = cells
                if starts_down:
                    cells, rr = [], r
                    while white(rr, c):
                        cells.append((rr, c)); rr += 1
                    down[k] = cells
    return numbers, across, down


class Board:
    def __init__(self, black, n):
        """black: 黒マス座標(row,col)の集合 / n: グリッドの一辺のマス数。"""
        self.black = set(black)
        self.n = n
        self.numbers, self.across, self.down = number_grid(self.black, self.n)
        self.fill = {}  # (r,c) -> char

    def is_white(self, r, c):
        return 0 <= r < self.n and 0 <= c < self.n and (r, c) not in self.black

    def place(self, num, direction, answer):
        """エントリに答えを置く。交差矛盾・長さ不一致があれば例外。"""
        entries = self.across if direction == "A" else self.down
        if num not in entries:
            raise KeyError(f"{num}{direction} は存在しません")
        cells = entries[num]
        if len(answer) != len(cells):
            raise ValueError(
                f"{num}{direction} 長さ不一致: 盤面{len(cells)}マス vs 答え'{answer}'{len(answer)}字"
            )
        for (rc, ch) in zip(cells, answer):
            if rc in self.fill and self.fill[rc] != ch:
                raise ValueError(
                    f"交差矛盾 @{rc}: 既存'{self.fill[rc]}' と '{ch}'({num}{direction})"
                )
        for (rc, ch) in zip(cells, answer):
            self.fill[rc] = ch

    def render(self):
        out = []
        for r in range(self.n):
            row = []
            for c in range(self.n):
                if (r, c) in self.black:
                    row.append("■")
                else:
                    row.append(self.fill.get((r, c), "・"))
            out.append(" ".join(row))
        return "\n".join(out)

    def missing(self):
        return [
            (r, c)
            for r in range(self.n)
            for c in range(self.n)
            if self.is_white(r, c) and (r, c) not in self.fill
        ]

    def verify_numbering(self, expected_across, expected_down):
        """導出した番号集合が、カギ一覧の番号集合と一致するか検査。

        構造(黒マス)読み取りの最重要チェック。一致しなければ黒マスが間違っている。
        """
        ok = set(self.across) == set(expected_across) and set(self.down) == set(expected_down)
        return ok, {
            "across_missing": sorted(set(expected_across) - set(self.across)),
            "across_extra": sorted(set(self.across) - set(expected_across)),
            "down_missing": sorted(set(expected_down) - set(self.down)),
            "down_extra": sorted(set(self.down) - set(expected_down)),
        }


if __name__ == "__main__":
    # 簡単な動作確認（架空の小さな盤面）
    demo_black = {(0, 2), (2, 0)}
    b = Board(black=demo_black, n=3)
    print("ヨコ:", {k: len(v) for k, v in sorted(b.across.items())})
    print("タテ:", {k: len(v) for k, v in sorted(b.down.items())})
