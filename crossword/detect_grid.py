#!/usr/bin/env python3
"""
盤面の黒マス自動検出ツール（汎用）

クロスワード写真から、グリッドの黒マス配置を検出して Board に渡せる形で出力する。
判定は「セル中央領域の輝度の中央値」で行う（黒塗りは中央値が非常に低い、
白マスは手書きがあっても中央値が高い → ノイズに強い）。

使い方:
    # 盤面の外枠 bbox を自動推定して 10x10 を検出、検証オーバーレイも出力
    python3 detect_grid.py photo.jpg --n 10 --overlay overlay.png

    # うまく外枠が取れない時は bbox を手動指定 (左,上,右,下 のピクセル)
    python3 detect_grid.py photo.jpg --bbox 1604 352 2259 980 --n 10

出力:
    - 各セルの median 輝度マトリクス（境界セルの目視確認用）
    - BLACK = {...} の Python リテラル（grid.Board(black=...) に渡す）
    - --overlay 指定時、検出結果を写真に重ねた PNG（必ず目視確認すること）
"""
from __future__ import annotations

import argparse

from PIL import Image, ImageDraw
import numpy as np


def auto_bbox(gray, search=None, thr=110):
    """暗い画素の射影から、グリッド外枠の bbox を粗く推定する。"""
    a = np.asarray(gray).astype(float)
    if search:
        sx0, sy0, sx1, sy1 = search
    else:
        sx0, sy0, sx1, sy1 = 0, 0, a.shape[1], a.shape[0]
    sub = a[sy0:sy1, sx0:sx1]
    dark = (sub < thr).astype(int)
    col = dark.sum(axis=0)
    row = dark.sum(axis=1)
    cs = np.where(col > col.max() * 0.45)[0]
    rs = np.where(row > row.max() * 0.45)[0]
    return (sx0 + cs.min(), sy0 + rs.min(), sx0 + cs.max(), sy0 + rs.max())


def detect(gray, bbox, n, black_thr=120):
    """各セルの median 輝度と黒マス集合を返す。"""
    a = np.asarray(gray).astype(float)
    x0, y0, x1, y1 = bbox
    W = (x1 - x0) / n
    H = (y1 - y0) / n
    med = [[0] * n for _ in range(n)]
    black = set()
    for r in range(n):
        for c in range(n):
            cy0 = int(y0 + (r + 0.22) * H); cy1 = int(y0 + (r + 0.78) * H)
            cx0 = int(x0 + (c + 0.22) * W); cx1 = int(x0 + (c + 0.78) * W)
            m = int(np.median(a[cy0:cy1, cx0:cx1]))
            med[r][c] = m
            if m < black_thr:
                black.add((r, c))
    return med, black, (W, H)


def overlay(img, bbox, n, black, path):
    x0, y0, x1, y1 = bbox
    W = (x1 - x0) / n; H = (y1 - y0) / n
    pad = 40
    crop = img.crop((max(0, x0 - pad), max(0, y0 - pad), x1 + pad, y1 + pad)).convert("RGB")
    ox, oy = max(0, x0 - pad), max(0, y0 - pad)
    d = ImageDraw.Draw(crop, "RGBA")
    for r in range(n):
        for c in range(n):
            X0 = x0 + c * W - ox; Y0 = y0 + r * H - oy
            d.rectangle([X0, Y0, X0 + W, Y0 + H], outline=(255, 0, 0, 255), width=1)
            if (r, c) in black:
                d.rectangle([X0, Y0, X0 + W, Y0 + H], fill=(255, 0, 0, 90))
            d.text((X0 + 2, Y0 + 1), f"{r},{c}", fill=(0, 0, 255, 255))
    crop = crop.resize((int(crop.width * 1.7), int(crop.height * 1.7)), Image.LANCZOS)
    crop.save(path)


def main():
    ap = argparse.ArgumentParser(description="盤面の黒マス自動検出")
    ap.add_argument("image")
    ap.add_argument("--n", type=int, default=10, help="グリッドのマス数(縦=横, 既定10)")
    ap.add_argument("--bbox", type=int, nargs=4, metavar=("X0", "Y0", "X1", "Y1"),
                    help="外枠ピクセル。省略時は自動推定")
    ap.add_argument("--search", type=int, nargs=4, metavar=("X0", "Y0", "X1", "Y1"),
                    help="自動推定の探索範囲(盤面が画像の一部のとき指定)")
    ap.add_argument("--black-thr", type=int, default=120, help="黒判定の輝度閾値")
    ap.add_argument("--overlay", help="検証オーバーレイPNGの出力先")
    args = ap.parse_args()

    img = Image.open(args.image)
    gray = img.convert("L")
    bbox = tuple(args.bbox) if args.bbox else auto_bbox(gray, args.search)
    print(f"bbox = {bbox}")
    med, black, (W, H) = detect(gray, bbox, args.n, args.black_thr)

    print("median 輝度 (低=黒)。120前後の境界値は要目視確認:")
    print("    " + " ".join(f"c{c}" for c in range(args.n)))
    for r in range(args.n):
        print(f"r{r} " + " ".join(f"{med[r][c]:3d}" for c in range(args.n)))

    print("\n# grid.Board(black=BLACK, n={}) に渡す:".format(args.n))
    items = ", ".join(f"({r},{c})" for (r, c) in sorted(black))
    print(f"BLACK = {{{items}}}")

    if args.overlay:
        overlay(img, bbox, args.n, black, args.overlay)
        print(f"\n検証オーバーレイを出力: {args.overlay}  ← 必ず目視確認")


if __name__ == "__main__":
    main()
