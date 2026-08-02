#include <stdbool.h>
#include <string.h>
#include <ctype.h>

bool digit_check(char *key) {
    int i = 0;

    while (i < (int)strlen(key) && isdigit(key[i])) {
        i++;
    }

    return i == (int)strlen(key);
}