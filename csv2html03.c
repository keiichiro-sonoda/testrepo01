// csvファイルをhtmlファイルに変換
// csvファイルのフォーマットは満たしているものとする
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// HTMLの冒頭の文字列
#define HTML_HEAD "<html><table border=1><tr><td>"

#define printDecimal(x) printf("%ld\n", (long)x);                // 10進数を表示するマクロ
#define printUDecimal(x) printf("%lu\n", (unsigned long long)x); // 符号なし10進数を表示するマクロ

// ある文字列に文字列を付け加える
// 結合後の文字列の最大長も渡す (ナル文字含む)
size_t addMoji(char *dst, const char *src, int dstmax) {
    size_t l;
    // 結合後の文字列が最大長と等しい，またはそれより大きかったらエラー
    if ((l = strlen(dst) + strlen(src)) >= dstmax) {
        printf("\aover\n");
        return -1;    // 返り値は size_t 型の最大値
    }
    strcat(dst, src); // 結合
    return l;         // 成功したら長さを返す
}

// 入力されたファイル名(パス)の拡張子を.htmlに変更
// 読み込むファイル名は必ず拡張子が付いていること前提
// 後から最初の . を探して，その後をhtmlにする
int makeOutFileName(char *dst, const char *src, int dstmax) {
    int i, l;
    char c;
    l = snprintf(dst, dstmax, "%s", src); // コピーして長さも取得
    // i をナル文字のひとつ前からデクリメント
    for (i = l - 1; (c = dst[i]) != '.'; i--) {
        // 途中で / が出たら拡張子が無いのでエラー
        // インデックスが負になる場合もエラー
        if (c == '/' || i < 0) {
            printf("\aエラー\n");
            return -1;
        }
    }
    dst[i + 1] = '\0';                   // . の直後をナル文字にする
    addMoji(dst, "html", dstmax);        // 拡張子変換
    printf("出力ファイル名: %s\n", dst); // 変換チェック
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) { // コマンド以外の引数は1つだけ指定
        puts("\a引数にcsvファイルのパスを1つ指定してください。");
        return -1;
    }
    FILE *fpr, *fpw;                   // ファイルのポインタ
    char c;                            // 読み込む文字
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];  // ファイル名 (入力, 出力)
    int dq_open = 0;                   // ダブルクォートが開いているフラグ
    int dq_first = 1;                  // 最初のダブルクォートかどうか
    int new_line = 0;                  // 改行直後
    // 最初の引数をファイル名に使う
    snprintf(fnamer, FILENAME_MAX, "%s", argv[1]);
    // HTMLファイル名に変更, 失敗した場合
    if (makeOutFileName(fnamew, fnamer, FILENAME_MAX) < 0) return -1;
    // ファイルを読込用で開き, 失敗した場合
    if ((fpr = fopen(fnamer, "r")) == NULL) {
        printf("\a%s can\'t be opened.\n", fnamer);
        return -1;
    }
    // ファイルを書込み用で開き, 失敗した場合
    if ((fpw = fopen(fnamew, "w")) == NULL) {
        printf("\a%s can\'t be opened.\n", fnamew);
        return -1;
    }
    fprintf(fpw, HTML_HEAD); // 冒頭の文字列の書込み
    // ファイルの終わりまで繰り返し
    while ((c = getc(fpr)) != EOF) {
        if (new_line) { // 改行直後がナル文字なら, 最終行にカウントしない
            if (c == '\0') break;
            else {      // ナル文字以外なら普通に改行
                fprintf(fpw, "</td></tr><tr><td>");
            }
            new_line = 0; // フラグを下ろす
        }
        if (dq_open) { // ダブルクォートが開いている場合
            if (c == '"') dq_open = 0; // ダブルクォートが出たら閉じる
            else if (c == '\n') {
                fprintf(fpw, "<br>");  // 改行が出たら <br> に置換
            } else {
                fputc(c, fpw);         // それ以外は全て文字として扱う
            }
        }
        else { // ダブルクォートが開いていない場合
            if (c == '"') { // ダブルクォートが出たら開く
                dq_open = 1;
                if (dq_first) {   // 最初のダブルクォートの場合
                    dq_first = 0; // フラグを下ろす
                } else {                  // 最初のダブルクォートでない場合
                    fputc('"', fpw);      // ダブルクォートを文字として扱う
                }
            } else if (c == ',') {  // カンマが出たら要素を区切る
                fprintf(fpw, "</td><td>");
                dq_first = 1;       // フラグリセット
            } else if (c == '\n') { // 改行
                new_line = 1;       // 改行フラグを立てる
            } else { // それ以外
                fputc(c, fpw);
            }
        }
    }
    // 最後の文字列を追加
    fprintf(fpw, "</td></tr></table></html>");

    fclose(fpr);               // 読込み用ファイルを閉じる
    fclose(fpw);               // 書込み用ファイルを閉じる
    return 0;
}