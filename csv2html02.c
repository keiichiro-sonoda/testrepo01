// csvファイルをhtmlファイルに変換
// csvファイルのフォーマットは満たしているものとする
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTML_MAX 1024  // HTML文字列の最大長

// HTMLの冒頭の文字列
#define HTML_HEAD "<html><style>table{table-layout:fixed;border-collapse:separate;color:black;width:80%;}table td{border:3px double black;}</style><table><tr><td>"

#define printDecimal(x) printf("%ld\n", (long)x);    // 10進数を表示するマクロ
#define printUDecimal(x) printf("%lu\n", (u_long)x); // 符号なし10進数を表示するマクロ

// ある文字列に文字列を付け加える
// 結合後の文字列の最大長も渡す (ナル文字含む)
int addMoji(char *dst, const char *src, int dstmax) {
    size_t l;
    // 結合後の文字列が最大長と等しい，またはそれより大きかったらエラー
    if ((l = strlen(dst) + strlen(src)) >= dstmax) {
        printf("\aover\n");
        return -1;
    }
    strcat(dst, src); // 結合
    return l;         // 成功したら長さを返す
}

// 入力されたファイル名(パス)の拡張子を.htmlに変更
// 読み込むファイル名は必ず拡張子が付いていること前提
// 後から最初の . を探して，その後をhtmlにする
int changeFileName(char *path, int pathmax) {
    int i, l;
    // 文字列の長さ(ナル文字の添え字)
    l = strlen(path);
    // i をナル文字のひとつ前からデクリメント
    for (i = l - 1; path[i] != '.'; i--) {
        // 途中で / が出たら拡張子が無いのでエラー
        if (path[i] == '/') {
            return -1;
        }
    }
    path[i + 1] = '\0';                   // . の直後をナル文字にする
    addMoji(path, "html", pathmax);       // . の後ろをhtmlにする
    printf("出力ファイル名: %s\n", path); // 変換チェック
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) { // コマンド以外の引数は1つだけ指定
        puts("\a引数にcsvファイルのパスを1つ指定してください。");
        return -1;
    }
    FILE *fp;                          // ファイルのポインタ
    char c;                            // 読み込む文字
    char fname[FILENAME_MAX];          // ファイル名 (入力, 出力)
    char html[HTML_MAX] = HTML_HEAD;   // 出力文字列 (冒頭はマクロで定義)
    size_t html_len = strlen(html);    // 出力文字列の長さ (ナル文字を含まない)
    int dq_open = 0;                   // ダブルクォートが開いているフラグ
    int dq_first = 1;                  // 最初のダブルクォートかどうか
    // 最初の引数をファイル名に使う
    snprintf(fname, FILENAME_MAX, "%s", argv[1]);
    // ファイルを読込用で開き, 失敗した場合
    if ((fp = fopen(fname, "r")) == NULL) {
        printf("\a%s can\'t be opened.\n", fname);
        return -1;
    }
    // ファイルの終わりまで繰り返し
    while ((c = getc(fp)) != EOF) {
        if (dq_open) { // ダブルクォートが開いている場合
            if (c == '"') dq_open = 0; // ダブルクォートが出たら閉じる
            else html[html_len++] = c; // それ以外は全て文字として扱う
        }
        else { // ダブルクォートが開いていない場合
            if (c == '"') { // ダブルクォートが出たら開く
                dq_open = 1;
                if (dq_first) {   // 最初のダブルクォートの場合
                    dq_first = 0; // フラグを下ろす
                } else {                  // 最初のダブルクォートでない場合
                    html[html_len++] = c; // ダブルクォートを文字として扱う
                }
            } else if (c == ',') { // カンマが出たら要素を区切る
                html_len = addMoji(html, "</td><td>", HTML_MAX);
                dq_first = 1;      // フラグリセット
            } else if (c == '\n') { // 改行
                html_len = addMoji(html, "</td></tr><tr><td>", HTML_MAX);
            } else { // それ以外
                html[html_len++] = c;
            }
        }
        // 出力文字列の長さがオーバーした場合
        if (html_len >= HTML_MAX) {
            printf("\a長さオーバー: %lu\n", html_len);
            html[HTML_MAX - 1] = '\0';
            puts(html);
            return -1;
        }
    }
    fclose(fp); // ファイルを閉じる
    // 最後の文字列を追加
    if ((html_len = addMoji(html, "</td></tr></table></html>", HTML_MAX)) >= HTML_MAX) {
        printf("\a長さオーバー: %lu\n", html_len);
        html[HTML_MAX - 1] = '\0';
        puts(html);
        return -1;
    }
    puts(html);
    // 長さチェック
    printf("HTMLの長さ: %ld \n", html_len);
    // HTMLファイル名に変更, 失敗した場合
    if (changeFileName(fname, FILENAME_MAX) < 0) return -1;
    if ((fp = fopen(fname, "w")) == NULL) { // 書き込み用で開く
        printf("\a%s can\'t be opened.\n", fname);
        return -1;
    }
    fprintf(fp, "%s", html);  // 開けたらファイルに書き込む
    fclose(fp);               // ファイルを閉じる
    return 0;
}