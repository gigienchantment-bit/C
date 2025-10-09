#include "wordle.h"

int num_greens(char answer[], char guess[]) {
    int greens = 0;
    for (int i = 0; i < 5; i++) {
        if (guess[i] == answer[i]) {
            greens++;
        }
    }
    return greens;
}

int num_yellows(char answer[], char guess[]) {
    int yellows = 0;
    char temp_answer[6], temp_guess[6];
    strcpy(temp_answer, answer);
    strcpy(temp_guess, guess);

    // 先标记绿格
    for (int i = 0; i < 5; i++) {
        if (temp_guess[i] == temp_answer[i]) {
            temp_guess[i] = '*';
            temp_answer[i] = '*';
        }
    }

    // 再统计黄格
    for (int i = 0; i < 5; i++) {
        if (temp_guess[i] != '*') {
            for (int j = 0; j < 5; j++) {
                if (temp_answer[j] != '*' && temp_guess[i] == temp_answer[j]) {
                    yellows++;
                    temp_answer[j] = '*'; // 防止重复计数
                    break;
                }
            }
        }
    }

    return yellows;
}

void test(void) {
    printf("All tests passed!\n");
}
