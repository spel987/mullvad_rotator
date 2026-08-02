#include <stdbool.h>
#include <stdio.h>
#include <time.h>

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

void connect_random_relay(int delay, bool only_owned) {
    int nb_relays = 0;

    //get the list of mullvad relay servers and store them in an array
    char *relay_list = get_relays_info();
    char **matchs = get_array_relays_list(relay_list, &nb_relays, only_owned);

    if (nb_relays == 0 || matchs == NULL) {
        printf("Error getting relays list\n");
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

        //format a command to pick the selected server
        int cmd_len = strlen("mullvad relay set location ") + strlen(matchs[random_number]) + 1;
        char *relay_set_cmd = malloc(cmd_len);
        snprintf(relay_set_cmd, cmd_len, "mullvad relay set location %s", matchs[random_number]);

        system(relay_set_cmd);

        free(relay_set_cmd);

        //connect to mullvad
        system("mullvad connect");

        printf("Switching relay in " COLOR_BOLD "%d secondes..." COLOR_OFF "\n--------------------\n\n", delay);

        SLEEP_SECONDS(delay);
    } while (is_running);

    
    free(relay_list);
    free_array_of_strings(matchs, nb_relays);
}