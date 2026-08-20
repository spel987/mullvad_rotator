#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

bool digit_check(char *key) {
    int i = 0;

    while (i < (int)strlen(key) && isdigit(key[i])) {
        i++;
    }

    return i == (int)strlen(key);
}

bool apply_mullvad_relay(char *cmd_format, char *relay_name) {
    char cmd[256];

    snprintf(cmd, sizeof(cmd), cmd_format, relay_name);

    char output[256] = {0};

    FILE *cmd_result = popen(cmd, "r");
    fgets(output, sizeof(output), cmd_result);
    pclose(cmd_result);

    return (strstr(output, "Relay constraints updated") != NULL);
}

void cleanup_and_exit(int sig) {
    printf("\nInterruption detected (sig number: %d)", sig);
    
    system("mullvad disconnect");
    
    printf("\nDisconnected from Mullvad\n");
    
    exit(0);
}