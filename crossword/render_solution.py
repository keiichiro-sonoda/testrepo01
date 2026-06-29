#!/usr/bin/env python3
"""
クロスワード完成図の作図ツール（汎用）

解答を記述した JSON を読み、黒マス・カギ番号・解答文字・二重枠を
matplotlib できれいに描いた PNG を出力する。特定の問題には依存しない。

    python3 render_solution.py spec.json out.png

spec.json の形式（例）:
{
  "n": 10,
  "black": [[0,1],[0,6], ...],            # 黒マス座標 [row,col]
  "across": {"2": "____", "5": "___", ...},# 番号 -> 解答（小書き仮名は大書き）
  "down":   {"1": "____", ...},
  "markers": {"A": [7,2], "B": [2,2], ...},# 二重枠ラベル -> [row,col]（任意）
  "title": "完成図",                        # 任意
  "subtitle": "二重枠 A→J ： ……"           # 任意
}

※ このリポジトリには問題固有の spec.json は含めない（解答データは公開しない）。
"""
import json
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib import font_manager

from grid import Board

_JP = None
for _fp in [
    "/usr/share/fonts/truetype/fonts-japanese-gothic.ttf",
    "/usr/share/fonts/opentype/ipafont-gothic/ipag.ttf",
]:
    try:
        _JP = font_manager.FontProperties(fname=_fp)
        break
    except Exception:
        pass


def board_from_spec(spec):
    n = spec["n"]
    black = {(r, c) for r, c in spec["black"]}
    b = Board(black=black, n=n)
    for num, word in spec.get("across", {}).items():
        b.place(int(num), "A", word)
    for num, word in spec.get("down", {}).items():
        try:
            b.place(int(num), "D", word)
        except Exception as e:
            print(f"[warn] {num}D: {e}")
    return b


def render(spec, out_path):
    b = board_from_spec(spec)
    n = b.n
    markers = {k: tuple(v) for k, v in spec.get("markers", {}).items()}
    marker_cell = {v: k for k, v in markers.items()}

    fig, ax = plt.subplots(figsize=(9, 9.8))
    ax.set_xlim(0, n); ax.set_ylim(0, n)
    ax.set_aspect("equal"); ax.invert_yaxis(); ax.axis("off")

    for r in range(n):
        for c in range(n):
            x, y = c, r
            if (r, c) in b.black:
                ax.add_patch(Rectangle((x, y), 1, 1, facecolor="#222222",
                                       edgecolor="#222222"))
                continue
            is_marker = (r, c) in marker_cell
            ax.add_patch(Rectangle((x, y), 1, 1,
                                   facecolor="#fff3c4" if is_marker else "white",
                                   edgecolor="#333333", linewidth=1.2))
            if is_marker:
                ax.add_patch(Rectangle((x + 0.07, y + 0.07), 0.86, 0.86,
                                       fill=False, edgecolor="#e8a000", linewidth=1.6))
                ax.text(x + 0.94, y + 0.93, marker_cell[(r, c)], ha="right",
                        va="bottom", fontsize=10, color="#c07000", fontweight="bold")
            num = b.numbers.get((r, c))
            if num:
                ax.text(x + 0.06, y + 0.05, str(num), ha="left", va="top",
                        fontsize=8.5, color="#555555")
            ch = b.fill.get((r, c), "")
            if ch:
                ax.text(x + 0.5, y + 0.58, ch, ha="center", va="center",
                        fontsize=26, color="#10305a", fontproperties=_JP)

    ax.add_patch(Rectangle((0, 0), n, n, fill=False, edgecolor="#222222", linewidth=2.5))

    if spec.get("title"):
        fig.suptitle(spec["title"], fontsize=20, fontproperties=_JP, y=0.975)
    if spec.get("subtitle"):
        ax.text(n / 2, n + 0.55, spec["subtitle"], ha="center", va="top",
                fontsize=15, color="#c0392b", fontproperties=_JP)

    fig.subplots_adjust(top=0.93, bottom=0.06)
    fig.savefig(out_path, dpi=150, bbox_inches="tight", facecolor="white")
    print("saved", out_path)


def main():
    if len(sys.argv) < 3:
        sys.exit("usage: python3 render_solution.py spec.json out.png")
    with open(sys.argv[1], encoding="utf-8") as f:
        spec = json.load(f)
    render(spec, sys.argv[2])


if __name__ == "__main__":
    main()
