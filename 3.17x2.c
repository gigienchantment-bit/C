#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

int num_greens(char answer[], char guess[]){
    int greens=0;
    for (int i = 0; i < 5; i++){
        if (guess[i] == answer[i]) greens++;
    }
    return greens;
}

int num_yellows(char answer[], char guess[]){
    char a[6], g[6];
    strcpy(a, answer);
    strcpy(g, guess);
    int yellows=0;
    for (int i = 0; i < 5; i++){
        if (g[i] == a[i])
        {g[i] = '*';
        a[i] = '*';}
    }
    for (int i = 0; i < 5; i++){
        if (g[i] != '*'){
            for (int j = 0; j < 5; j++){
                if (a[j] != '*' && g[i] == a[j]){
                    yellows++;
                    a[j] = '*';
                    break;
                }
            }
        }
    }
    return yellows;
}

void test(void){
    char answer[6];
    printf("answer");
    scanf("%5s",answer);
    char guess[6];
    printf("guess");
    scanf("%5s",guess);
    for (int i=0;i<5;i++){
        answer[i]=toupper(answer[i]);
    }
    for (int i=0;i<5;i++){
        guess[i]=toupper(guess[i]);
    }
    int greens=num_greens(answer, guess);
    int yellows=num_yellows(answer, guess);
    printf("Greens:%d;Yellows:%d",greens,yellows);

}

int main(void) {
    char word1[6] = "HELLO";
    char word2[6] = "WORLD";

    // 'L' is green, 'O' is yellow
    assert(num_greens(word1, word2) == 1);
    assert(num_yellows(word1, word2) == 1);

    strcpy(word1, "APPLE");
    strcpy(word2, "ALLEY");

    // 'A' is green, 'L' and 'E' are yellow
    assert(num_greens(word1, word2) == 1);
    assert(num_yellows(word1, word2) == 2);

    // Should give same result if answer and guess swapped
    assert(num_greens(word2, word1) == 1);
    assert(num_yellows(word2, word1) == 2);

    test();

    return 0;
}

// 🌟 视频&下面的内容于2025年1月3日更新（弹幕在更新视频后被我清理过一次了）
// command line tools安装命令（可以不安装Xcode，下载慢可以尝试换网络或者热点）：xcode-select --install
// Visual Studio Code软件下载地址：https://code.visualstudio.com/
// 关闭C/C++扩展的设置：Debug Shortcut
// 调试替换program（注意外层有双引号）：${fileDirname}/${fileBasenameNoExtension}
// 开箱即用：https://www.123pan.com/s/5FJqjv-DSGYd.html
// ===补充说明的文章===
// 🌟 支持C/C++11：关于Mac版本的Visual Studio Code如何支持C/C++11
// ===分割线===
// ❗️扩展设置和Visual Studio Code设置只需要设置一次
// ❗️调试和编译功能跟随文件夹，因此如果在新的文件夹中需要编译和调试，可以去云盘上再下载一次模板
// ❗️上面所有的命令、链接等不要手动输入，直接复制，手动输入可能存在问题
// ❗️希望各位可以的话看完视频，视频后面有常见问题Q&A