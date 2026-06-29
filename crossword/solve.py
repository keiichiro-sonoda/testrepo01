#!/usr/bin/env python3
"""
クロスワード自動解答ツール

新聞などのクロスワードパズルを撮影した写真を1枚渡すと、
Claude(vision) がマス目・カギ(手がかり)を読み取り、各カギの答えを推論し、
盤面を全マス埋めて返します。日本語のカタカナ/ひらがなクロスワードを想定。

使い方:
    export ANTHROPIC_API_KEY=sk-ant-...
    python3 solve.py photo.jpg
    python3 solve.py photo.jpg --json answer.json   # 構造化結果も保存
    python3 solve.py photo.jpg --model claude-sonnet-4-6  # 安いモデルで

依存:
    pip install anthropic
"""
from __future__ import annotations

import argparse
import base64
import json
import mimetypes
import os
import sys
import textwrap


# 既定は最新かつ最も賢いモデル(vision対応・推論力が高い)。
# 安く済ませたい場合は --model claude-sonnet-4-6 / claude-haiku-4-5-20251001
DEFAULT_MODEL = "claude-opus-4-8"

SYSTEM_PROMPT = """\
あなたは日本の新聞クロスワードパズルを解く専門家です。
渡された写真(複数枚のことがあります。同じパズルの全体写真と接写など)には、
クロスワードの盤面(マス目)と、「タテのカギ」「ヨコのカギ」の手がかり文が写っています。
複数枚ある場合は同一パズルの別アングル/接写とみなし、情報を統合してください。
あなたの仕事は次の手順を厳密に行うことです。

1. 盤面を読み取る。何行×何列か、黒マス(塗りつぶし)の位置、各白マスの左上に
   振られた番号(カギ番号)、すでに文字が書き込まれているマスを把握する。
   写真に手書きで書き込まれた既存の文字があれば、それも答えの根拠として尊重する。
2. 「タテのカギ」「ヨコのカギ」の各手がかり文を番号付きで全て読み取る。
3. 各カギの答えを、日本語の一般常識・時事・固有名詞の知識を使って推論する。
   答えは通常カタカナ(問題によってはひらがな)で、マス数とぴったり一致させる。
4. タテとヨコで交差するマスの文字が必ず一致するよう、クロスワードの制約を満たす
   唯一の整合解を求める。1文字でも矛盾があれば推論を見直す。
5. 「二重枠」や「A〜J をアルファベット順に並べ替える」等のボーナス問題が
   写真にあれば、それも解いて answer_phrase に入れる。

重要: 出力は説明文を一切付けず、次のJSONオブジェクトだけを返すこと(コードフェンス不要)。

{
  "grid": {
    "rows": <行数:int>,
    "cols": <列数:int>,
    "cells": [["文字 or null(黒マス) or 空文字(未確定)", ...], ...]
  },
  "across": [{"number": <int>, "clue": "<手がかり文>", "answer": "<カタカナ等>"}, ...],
  "down":   [{"number": <int>, "clue": "<手がかり文>", "answer": "<カタカナ等>"}, ...],
  "answer_phrase": "<ボーナスの最終回答。無ければ空文字>",
  "confidence": "<high|medium|low>",
  "notes": "<読み取りや推論で曖昧だった点。無ければ空文字>"
}

cells は盤面全体の二次元配列。黒マスは null、白マスは確定した1文字、
どうしても確定できない白マスは "" を入れる。可能な限り全マスを埋めること。
"""

USER_PROMPT = """\
この写真のクロスワードを解いてください。盤面・カギを読み取り、各カギの答えを推論し、
タテヨコの交差が全て一致するように盤面を全マス埋めて、指定のJSON形式だけで返してください。\
"""


def encode_image(path: str) -> tuple[str, str]:
    """画像を読み込み (media_type, base64文字列) を返す。"""
    if not os.path.isfile(path):
        sys.exit(f"エラー: 画像が見つかりません: {path}")
    media_type, _ = mimetypes.guess_type(path)
    if media_type not in ("image/jpeg", "image/png", "image/gif", "image/webp"):
        # 拡張子から判定できない場合は jpeg 扱い
        media_type = "image/jpeg"
    with open(path, "rb") as f:
        data = base64.standard_b64encode(f.read()).decode("ascii")
    return media_type, data


def call_claude(image_paths: list[str], model: str) -> dict:
    try:
        import anthropic
    except ImportError:
        sys.exit("エラー: anthropic が必要です。`pip install anthropic` を実行してください。")

    if not (os.environ.get("ANTHROPIC_API_KEY") or os.environ.get("ANTHROPIC_AUTH_TOKEN")):
        sys.exit("エラー: 環境変数 ANTHROPIC_API_KEY を設定してください。")

    # 複数枚(盤面の接写・カギの接写など)を渡せる。小さい文字は分割撮影すると精度が上がる。
    content: list[dict] = []
    for path in image_paths:
        media_type, b64 = encode_image(path)
        content.append(
            {
                "type": "image",
                "source": {"type": "base64", "media_type": media_type, "data": b64},
            }
        )
    content.append({"type": "text", "text": USER_PROMPT})

    client = anthropic.Anthropic()
    resp = client.messages.create(
        model=model,
        max_tokens=8000,
        system=SYSTEM_PROMPT,
        messages=[{"role": "user", "content": content}],
    )
    text = "".join(block.text for block in resp.content if block.type == "text")
    return parse_json(text)


def parse_json(text: str) -> dict:
    """モデル出力からJSONを取り出す(コードフェンスや前後の説明文を許容)。"""
    text = text.strip()
    if text.startswith("```"):
        # ```json ... ``` を剥がす
        text = text.split("```", 2)[1]
        if text.startswith("json"):
            text = text[4:]
        text = text.strip().rstrip("`").strip()
    # 最初の { から最後の } までを抽出
    start, end = text.find("{"), text.rfind("}")
    if start == -1 or end == -1:
        sys.exit(f"エラー: 応答からJSONを抽出できませんでした。\n--- 応答 ---\n{text}")
    try:
        return json.loads(text[start : end + 1])
    except json.JSONDecodeError as e:
        sys.exit(f"エラー: JSON解析に失敗しました: {e}\n--- 応答 ---\n{text}")


def render_grid(grid: dict) -> str:
    """盤面をターミナル表示用のテキストに整形する。"""
    cells = grid.get("cells", [])
    if not cells:
        return "(盤面データなし)"
    lines = []
    for row in cells:
        rendered = []
        for cell in row:
            if cell is None:
                rendered.append("■")        # 黒マス
            elif cell == "":
                rendered.append("・")        # 未確定
            else:
                rendered.append(str(cell))
        lines.append(" ".join(rendered))
    return "\n".join(lines)


def render_clues(title: str, clues: list[dict]) -> str:
    out = [f"【{title}】"]
    for c in sorted(clues, key=lambda x: x.get("number", 0)):
        out.append(f"  {c.get('number'):>2}. {c.get('answer','?'):<10} … {c.get('clue','')}")
    return "\n".join(out)


def main() -> None:
    ap = argparse.ArgumentParser(description="クロスワード自動解答ツール")
    ap.add_argument(
        "images",
        nargs="+",
        help="クロスワードを撮影した画像。複数指定可(例: 盤面の接写 カギの接写)。"
        "小さい文字は分割撮影すると精度が上がります。",
    )
    ap.add_argument("--model", default=DEFAULT_MODEL, help=f"使用モデル (既定: {DEFAULT_MODEL})")
    ap.add_argument("--json", metavar="FILE", help="構造化結果(JSON)の保存先")
    args = ap.parse_args()

    print(f"画像{len(args.images)}枚を解析中… (model={args.model})", file=sys.stderr)
    result = call_claude(args.images, args.model)

    print("\n=== 盤面(完成) ===")
    print(render_grid(result.get("grid", {})))
    print()
    print(render_clues("ヨコのカギ", result.get("across", [])))
    print()
    print(render_clues("タテのカギ", result.get("down", [])))

    phrase = result.get("answer_phrase", "")
    if phrase:
        print(f"\n★ 最終回答(ボーナス): {phrase}")
    conf = result.get("confidence", "")
    if conf:
        print(f"\n信頼度: {conf}")
    notes = result.get("notes", "")
    if notes:
        print("メモ: " + textwrap.fill(notes, 60, subsequent_indent="      "))

    if args.json:
        with open(args.json, "w", encoding="utf-8") as f:
            json.dump(result, f, ensure_ascii=False, indent=2)
        print(f"\n構造化結果を保存しました: {args.json}", file=sys.stderr)


if __name__ == "__main__":
    main()
