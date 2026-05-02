#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int words_parser(char ***words) {
    FILE *f = fopen("tests/sortlist.txt", "r");
    if (!f) return -1;

    int capacity = 16;
    int cnt = 0;
    *words = (char **) malloc(capacity * sizeof(char *));
    if (!*words) {
        fclose(f);
        return -1;
    }

    char *buffer = NULL;
    int buf_cap = 0;
    int chars_cnt = 0;
    int c;

    while ((c = fgetc(f)) != EOF) {
        if (!isspace(c) && !ispunct(c)) {
            if (chars_cnt + 1 >= buf_cap) {
                int next_cap = buf_cap == 0 ? 16 : buf_cap * 2;
                char *new_buf = (char *) realloc(buffer, next_cap);
                if (!new_buf) {
                    free(buffer);
                    for (int i = 0; i < cnt; i++) free((*words)[i]);
                    free(*words);
                    fclose(f);
                    return -1;
                }
                buffer = new_buf;
                buf_cap = next_cap;
            }
            if (buffer != NULL) buffer[chars_cnt++] = (char) c;
        } else if (chars_cnt > 0) {
            if (buffer) {
                buffer[chars_cnt] = '\0';
                if (cnt >= capacity) {
                    capacity *= 2;
                    char **new_words = (char **) realloc(*words, capacity * sizeof(char *));
                    if (!new_words) {
                        free(buffer);
                        for (int i = 0; i < cnt; i++) free((*words)[i]);
                        free(*words);
                        fclose(f);
                        return -1;
                    }
                    *words = new_words;
                }
                (*words)[cnt++] = strdup(buffer);
                chars_cnt = 0;
            }
        }
    }

    if (chars_cnt > 0 && buffer) {
        buffer[chars_cnt] = '\0';
        if (cnt >= capacity) {
            char **new_words = (char **) realloc(*words, (cnt + 1) * sizeof(char *));
            if (new_words) {
                *words = new_words;
                (*words)[cnt++] = strdup(buffer);
            }
        } else {
            (*words)[cnt++] = strdup(buffer);
        }
    }

    free(buffer);
    fclose(f);
    return cnt;
}


void shaker_sort(char **words, int n, int (*cmp)(const char *, const char *)) {
    int left = 0;
    int right = n - 1;
    int swapped = 1;

    while (left < right && swapped) {
        swapped = 0;
        for (int i = left; i < right; i++) {
            if (cmp(words[i], words[i + 1]) > 0) {
                char *tmp = words[i];
                words[i] = words[i + 1];
                words[i + 1] = tmp;
                swapped = 1;
            }
        }
        right--;

        for (int i = right; i > left; i--) {
            if (cmp(words[i - 1], words[i]) > 0) {
                char *tmp = words[i];
                words[i] = words[i - 1];
                words[i - 1] = tmp;
                swapped = 1;
            }
        }
        left++;
    }
}


int binary_search(char **words, int n, const char *target, int (*cmp)(const char *, const char *)) {
    int left = 0;
    int right = n - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp_res = cmp(words[mid], target);

        if (cmp_res == 0) return mid;
        if (cmp_res < 0) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

int main() {
    char **words = NULL;
    int words_cnt = words_parser(&words);
    if (words_cnt <= 0) {
        return 1;
    }

    shaker_sort(words, words_cnt, strcmp);
    int index;
    printf("Enter the key (1 to %d): ", words_cnt);
    if (scanf("%d", &index) == 1) {
        if (index >= 1 && index <= words_cnt) {
            printf("Found %d: %s\n", index, words[index - 1]);
        } else {
            printf("Error: bad index\n");
        }
    }
    for (int i = 0; i < words_cnt; i++) {
        free(words[i]);
    }
    free(words);
    return 0;
}