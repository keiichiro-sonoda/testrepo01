#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIN_MAX  256   // 読み込むファイルの1行当たりの最大文字数
#define ROW_MAX  10    // 最大行数
#define COL_MAX  10    // 最大列数
#define ELE_MAX  16    // 各要素の最大文字数
#define HTML_MAX 1024  // HTML文字列の最大長

// 文字の配列をメンバーに持つ型を定義
typedef struct mojiretsu {
    char str[ELE_MAX];
} Mojiretsu;

// さらに Mojiretsu 型の配列をもつ型を定義
typedef struct gyo {
    Mojiretsu row[COL_MAX];
    // 各行の要素数
    int row_length;
} Gyo;

// 望ましい配列に変換する関数
// 各変数が最大値を越えた時点でエラー
// ダブルクォートで囲わない(先頭がダブルクォートでない)場合，
// 要素にはカンマやダブルクォートが存在しないことを前提とする
// フォーマットに沿わない場合もエラー
int csvToArray(const char *src, Mojiretsu *dst) {
    int i;     // イテレータ変数
    int j = 0; // 要素番号
    int k = 0; // 出力文字列の添え字
    // 各種フラグ
    int top_dq = 0;       // 先頭がダブルクォートかどうか
    int dq_close = 0;     // ダブルクォートが閉じているかどうか
    // 入力文字列を先頭から追っていく
    // ナル文字か改行文字の場合終了
    for (i = 0; src[i] != '\0' && src[i] != '\n' && src[i] != '\r'; i++) {
        // ダブルクォートが出現した場合
        if (src[i] == '\"') {
            // 文字列の1文字目の場合
            // 先頭のダブルクォートの場合
            if (k == 0 && top_dq == 0) {
                // 先頭がダブルクォートのフラグを立てる
                top_dq = 1;
            }  // 先頭以外のダブルクォート
            else {
                // 先頭にダブルクォートがある場合
                if (top_dq == 1) {
                    // ダブルクォートが閉じている場合(2回連続ダブルクォート)
                    if (dq_close == 1) {
                        // 書込み先が配列の最後尾またはそれ以上のとき，エラー
                        if (k >= ELE_MAX - 1) return -1;
                        // 出力文字列にダブルクォートを追加
                        dst[j].str[k] = '\"';
                        // 出力文字列の添え字をインクリメント
                        k++;
                        // 文字列扱いなのでダブルクォートは閉じていない
                        dq_close = 0;
                    } // ダブルクォートが開いてる場合
                    else {
                        // ダブルクォートが閉じたフラグを立てる
                        // 同時に，次にダブルクォートが来た時のエスケープフラグとして扱う
                        dq_close = 1;
                    }
                } // 先頭がダブルクォートでない
                else {
                    // エラーを示す値を返す
                    return -3;
                }
            }
        } // カンマが出現
        else if (src[i] == ',') {
            // 先頭がダブルクォートでかつ，ダブルクォートが閉じていない場合
            // この条件下のみ，カンマを文字として扱う
            if (top_dq == 1 && dq_close == 0) {
                // 書込み先が配列の最後尾またはそれ以上のとき，エラー
                if (k >= ELE_MAX - 1) return -1;
                // 出力文字列にカンマを追加
                dst[j].str[k] = ',';
                // 出力文字列の添え字をインクリメント
                k++;
            } // 上記以外，カンマを区切り文字として扱う
            else {
                // 出力文字列はナル文字で終える
                dst[j].str[k] = '\0';
                // 次の要素へ
                j++;
                // 最大列数を越えた場合，エラー
                if (j >= COL_MAX) return -2;
                // 出力文字列の添え字をリセット
                k = 0;
                // 各種フラグをリセット
                top_dq = 0;
                dq_close = 0;
            }
        } // その他の文字
        else {
            // ダブルクォートが閉じている場合，エラー
            if (dq_close == 1) return -3;
            // ダブルクォートが閉じていない場合
            else {
                // 書込み先が配列の最後尾またはそれ以上のとき，エラー
                if (k >= ELE_MAX - 1) return -1;
                // そのまま文字列に追加
                dst[j].str[k] = src[i];
                // 出力文字の添え字インクリメント
                k++;
            }
        }
    }
    // ダブルクォートで閉じずに終わった場合，エラー
    if (top_dq == 1 && dq_close == 0) return -3;
    // 最後の要素の最後尾もナル文字に
    dst[j].str[k] = '\0';
    // 変換に成功したら要素数を返す
    return j + 1;
}

// 表を可視化する関数
// Gyo型の配列と行数を与える
int printTable(Gyo *l_table, int l_row_count) {
    // イテレータ変数
    int i, j;
    printf("preview:\n");
    // 行数繰り返し
    for (i = 0; i < l_row_count; i++) {
        // 各行の要素数だけ繰り返す
        for (j = 0; j < l_table[i].row_length; j++) {
            // 指定文字数左詰めで表示し、空白で区切る
            printf("%16s ", l_table[i].row[j].str);
        }
        // 改行
        printf("\n");
    }
    return 0;
}

// ある文字列に文字列を付け加える
// 結合後の文字列の最大長も渡す(ナル文字含む)
int addMoji(char *dst, const char *src, int dstmax) {
    // 結合後の文字列が最大長とぴったり，またはそれより大きかったらエラー
    if (strlen(dst) + strlen(src) >= dstmax) {
        printf("over\n");
        return -1;
    }
    // 結合
    strcat(dst, src);
    return 0;
}

// 配列からhtmlに変換する関数
// 引数には表，行数，出力文字列，出力文字列の最大長を与える
int arrayToHtml(Gyo *l_table, int l_row_count, char *l_html, int l_html_max) {
    int i, j;
    // 冒頭の文字列 (表のフォーマットなども指定)
    addMoji(l_html, "<html><style>table{table-layout:fixed;border-collapse:collapse;color:black;width:80%;}table td {border: 1px solid black;}</style><table>", l_html_max);
    // 行数繰り返し
    for (i = 0; i < l_row_count; i++) {
        addMoji(l_html, "<tr>", l_html_max); // 行の始まり
        // 要素数繰り返し(行ごとに異なる)
        for (j = 0; j < l_table[i].row_length; j++) {
            addMoji(l_html, "<td>", l_html_max);                // 要素の始まり
            addMoji(l_html, l_table[i].row[j].str, l_html_max); // 要素
            addMoji(l_html, "</td>", l_html_max);               // 要素の終わり
        }
        addMoji(l_html, "</tr>", l_html_max); // 行の終わり
    }
    addMoji(l_html, "</table></html>", l_html_max); // 文末
    // 長さチェック
    printf("HTML length is %ld.\n", strlen(l_html));
    return 0;
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
    // . の直後をナル文字にする
    path[i + 1] = '\0';
    // . の後ろをhtmlにする
    addMoji(path, "html", pathmax);
    // 変換チェック
    printf("Output file path: %s\n", path);
    return 0;
}

// 標準入力から文字列を受け取る
// 読み込みファイル名を入力するのに用いる
int input(char *buf, int bufmax) {
    int i;
    // 入力を促す文字列
    printf("Input csv path: ");
    // fgetsで読み込み
    fgets(buf, bufmax, stdin);
    // 改行文字まで含むのでナル文字に変換
    for (i = 0; !(buf[i] == '\0' || buf[i] == '\n' || buf[i] == '\r'); i++);
    buf[i] = '\0';
    // 文字列チェック
    printf("%s\n", buf);
    return 0;
}

// こっからメイン
int main(int argc, char **argv) {
    FILE *fp;                // FILE型構造体
    char line[LIN_MAX];      // ファイルの各行の文字列を一時的に保存
    int row_count = 0;       // 行数のカウント
    int e;
    Gyo table[ROW_MAX];      // Gyo型の配列(行数分)
    // 出力文字列(初期はナル文字だけ)
    char html[HTML_MAX] = {'\0'};
    // ファイルのパス
    char fname[LIN_MAX];
    // 最初の引数をファイル名に使う
    snprintf(fname, LIN_MAX, "%s", argv[1]);
    puts(fname);
    printf("%d\n", argc);
    // ファイルを読込用で開く
    // 失敗した場合
    if ((fp = fopen(fname, "r")) == NULL) {
        printf("%s can\'t be opened.\n", fname);
        return -1;
    }
    // fgets で行が無くなるまで繰り返す
    while (fgets(line, LIN_MAX, fp) != NULL) {
	if (row_count >= ROW_MAX) {
	    printf("row over\n");
	    return -1;
        }
        // 行ごと読み込んで配列に変換
        // ついでに要素数も計算する
        // 負の値が返ってきたらエラーを表示して終了
	    e = csvToArray(line, table[row_count].row);
        if (e == -1) {
            printf("element over\n");
            return -1;
	    } else if (e == -2) {
            printf("column over\n");
            return -1;
        } else if (e == -3) {
            printf("invalid format\n");
            return -1;
        }
        table[row_count].row_length = e;
        printf("line%2d: The number of elements is %2d.\n", row_count + 1, table[row_count].row_length);
        // 行の数をインクリメント
        row_count++;
    }
    // ファイルを閉じる
    fclose(fp);
    // 行数チェック
    printf("The number of lines is %d.\n", row_count);
    // 望ましい配列になっているかチェック
    printTable(table, row_count);
    // htmlに変換
    arrayToHtml(table, row_count, html, HTML_MAX);

    // 出力ファイル名に変更
    changeFileName(fname, sizeof(fname));
    // 書込み用で開く
    if ((fp = fopen(fname, "w")) == NULL) {
        printf("%s can\'t be opened.\n", fname);
        return -1;
    }
    // 無事開けたらファイル書込み
    fprintf(fp, "%s", html);
    fclose(fp);

    return 0;
}
