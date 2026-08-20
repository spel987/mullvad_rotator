#ifndef UTILS_H
#define UTILS_H

#define COLOR_BOLD "\e[1m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_OFF "\e[m"

#include <stdbool.h>

bool digit_check(char *key);
bool apply_mullvad_relay(char *cmd_format, char *relay_name);

#endif