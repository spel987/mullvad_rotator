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

bool check_mullvad_availability() {
    int capacity = 1024;

    char *buffer = malloc(capacity * sizeof(char));

    if (buffer == NULL) {
        return false;
    }

    FILE *cmd = popen("mullvad version 2>&1", "r");

    if (cmd == NULL) {
        free(buffer);
        return false;
    }

    while (fgets(buffer, capacity, cmd) != NULL) {
        // drain remaining output
    }

    free(buffer);
    int value_pclose = pclose(cmd);

    return value_pclose == 0;
}

bool check_mullvad_account() {
    int capacity = 1024;

    char *buffer = malloc(capacity * sizeof(char));

    if (buffer == NULL) {
        return false;
    }

    FILE *cmd = popen("mullvad account get 2>&1", "r");

    if (cmd == NULL) {
        free(buffer);
        return false;
    }

    char *first_line = fgets(buffer, capacity, cmd);

    if (first_line == NULL) return false;
    
    bool account_connected = strstr(buffer, "Not logged in on any account") == NULL;

    while (fgets(buffer, capacity, cmd) != NULL) {
        // drain remaining output
    }

    free(buffer);
    pclose(cmd);

    return account_connected;
}