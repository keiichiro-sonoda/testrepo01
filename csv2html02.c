#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIN_MAX  256   // 読み込むファイルの1行当たりの最大文字数
#define HTML_MAX 1024  // HTML文字列の最大長

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
        puts("引数にcsvファイルのパスを1つ指定してください。");
        return -1;
    }
    FILE *fp;                // FILE型構造体
    // 出力文字列(初期はナル文字だけ)
    char html[HTML_MAX] = {'\0'};
    // 最初の引数をファイル名に使う
    // ファイルを読込用で開き, 失敗した場合
    if ((fp = fopen(argv[1], "r")) == NULL) {
        printf("%s can\'t be opened.\n", argv[1]);
        return -1;
    }
    fclose(fp); // ファイルを閉じる
    puts("終わり");
    return 0;
}
