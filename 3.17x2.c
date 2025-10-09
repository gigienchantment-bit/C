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
