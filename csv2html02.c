#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIN_MAX  256   // 読み込むファイルの1行当たりの最大文字数
#define HTML_MAX 1024  // HTML文字列の最大長

#define printDecimal(x) printf("%ld\n", (long)x); // 10進数を表示するマクロ

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
    // . の直後をナル文字にする
    path[i + 1] = '\0';
    // . の後ろをhtmlにする
    addMoji(path, "html", pathmax);
    // 変換チェック
    printf("Output file path: %s\n", path);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) { // コマンド以外の引数は1つだけ指定
        puts("\a引数にcsvファイルのパスを1つ指定してください。");
        return -1;
    }
    FILE *fp;                     // ファイルのポインタ
    char c;                       // 読み込む文字
    char html[HTML_MAX] = {'\0'}; // 出力文字列 (初期はナル文字だけ)
    int dq;                       // ダブルクォートが開いているフラグ
    // 最初の引数をファイル名に使う
    // ファイルを読込用で開き, 失敗した場合
    if ((fp = fopen(argv[1], "r")) == NULL) {
        printf("\a%s can\'t be opened.\n", argv[1]);
        return -1;
    }
    // ファイルの終わりまで繰り返し
    while ((c = getc(fp)) != EOF) {
        if (c == '"') {
            dq ^= 1;
        } else {
            putchar(c);
        }
    }
    putchar(10);
    fclose(fp); // ファイルを閉じる
    char str1[7] = "abc", str2[5] = "def";
    printDecimal(addMoji(str1, str2, 3));
    puts(str1);
    puts("終わり");
    return 0;
}
