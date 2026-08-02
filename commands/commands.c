#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "../relay_data/relay_data.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

int get_relay_count(bool only_owned) {
    int nb_relays = 0;

    //get the list of mullvad relay servers and store them in an array
    char *relay_list = get_relays_info();
    char **matchs = get_array_relays_list(relay_list, &nb_relays, only_owned);

    free(relay_list);
    free_array_of_strings(matchs, nb_relays);

    return nb_relays;
}

void print_relays_list_formatted(bool only_owned) {
    int nb_relays = 0;
    
    //get the list of mullvad relay servers and store them in an array
    char *relay_list = get_relays_info();
    char **matchs = get_array_relays_list(relay_list, &nb_relays, only_owned);

    printf("List of the %d available with your option", nb_relays);

    if (only_owned) {
        printf(" (only owned by Mullvad)\n\n");
    } else {
        printf("\n\n");
    }

    //browse the array to display the name of each relay
    for (int i = 0; i < nb_relays; i++) {
        printf("%s\n", matchs[i]);
    }

    free(relay_list);
    free_array_of_strings(matchs, nb_relays);
}

bool connect_relay(char *relay_name, int delay) {
    //format a command to pick the selected server
    int cmd_len = strlen("mullvad relay set location ") + strlen(relay_name) + 6;
    char *relay_set_cmd = malloc(cmd_len);
    snprintf(relay_set_cmd, cmd_len, "mullvad relay set location %s 2>&1", relay_name);

    char output[256];
    FILE *cmd = popen(relay_set_cmd, "r");

    fgets(output, sizeof(output), cmd);
    pclose(cmd);

    if (strstr(output, "Relay constraints updated") != NULL) {
        //connect to mullvad
        printf(COLOR_GREEN "Relay server change completed successfully\n" COLOR_OFF);
        system("mullvad connect");
        printf("Switching relay in " COLOR_BOLD "%d secondes..." COLOR_OFF "\n--------------------\n\n", delay);
        
    } else {
        printf(COLOR_RED "Error: invalid relay server name" COLOR_OFF "\n--------------------");
        free(relay_set_cmd);
        return false;
    }

    free(relay_set_cmd);
    return true;
}

void connect_random_relay(int delay, bool only_owned) {
    int nb_relays = 0;

    //get the list of mullvad relay servers and store them in an array
    char *relay_list = get_relays_info();
    char **matchs = get_array_relays_list(relay_list, &nb_relays, only_owned);

    if (nb_relays == 0 || matchs == NULL) {
        printf("Error getting relays list\n");
        free(relay_list);
        free_array_of_strings(matchs, nb_relays);
        return;
    }

    bool is_running = true;

    do {
        //get a random relay server
        srand(time(NULL));
        int random_number = (rand() % (nb_relays));
        printf("--------------------\nRandom relay picked: " COLOR_BOLD COLOR_GREEN "%s" COLOR_OFF, matchs[random_number]);

        if (only_owned) {
            printf(" (owned by Mullvad)\n--------------------\n");
        } else {
            printf("\n--------------------\n");
        }

        if (!connect_relay(matchs[random_number], delay)) {
            break;
        }

        SLEEP_SECONDS(delay);
    } while (is_running);

    
    free(relay_list);
    free_array_of_strings(matchs, nb_relays);
}

void connect_specific_relay(int delay, char **relay_names, int nb_relays) {
    bool is_running = true;

    do {
        //get a random relay server
        srand(time(NULL));
        int random_number = (rand() % (nb_relays));
        printf("--------------------\nRandom relay picked: " COLOR_BOLD COLOR_GREEN "%s" COLOR_OFF "\n--------------------\n", relay_names[random_number]);

        if (!connect_relay(relay_names[random_number], delay)) {
            break;
        }

        SLEEP_SECONDS(delay);
    } while (is_running);
}