#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "../relay_data/relay_data.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

int get_relay_count(bool only_owned, char *country_tag) {
    int nb_relays = 0;

    //get the list of mullvad relay servers and store them in an array
    char *relay_list = get_relays_info();
    char **matchs = get_array_relays_list(relay_list, &nb_relays, only_owned, country_tag);

    free(relay_list);
    free_array_of_strings(matchs, nb_relays);

    return nb_relays;
}

void print_relays_list_formatted(bool only_owned, char *country_tag) {
    int nb_relays = 0;
    
    //get the list of mullvad relay servers and store them in an array
    char *relay_list = get_relays_info();
    char **matchs = get_array_relays_list(relay_list, &nb_relays, only_owned, country_tag);

    printf("List of the %d available with your option(s)", nb_relays);

    if (only_owned && country_tag[0] != '\0') {
        printf(" (only owned by Mullvad and corresponding to country tag \"%s\")\n\n", country_tag);
    } else if (only_owned) {
        printf(" (only owned by Mullvad)\n\n");
    } else if (country_tag[0] != '\0') {
        printf(" (corresponding to country tag \"%s\")\n\n", country_tag);
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
    //disable multihop by default
    bool disable_multihop = apply_mullvad_relay("mullvad relay set multihop off", "");

    if (!disable_multihop) {
        printf(COLOR_RED "Error: unable to disable multihop mode\n" COLOR_OFF);
        return false;
    }

    bool apply_relay = apply_mullvad_relay("mullvad relay set location %s 2>&1", relay_name);

    if (apply_relay) {
        //connect to mullvad
        printf(COLOR_GREEN "Relay server change completed successfully\n" COLOR_OFF);
        system("mullvad connect");
        printf("Switching relay in " COLOR_BOLD "%d secondes..." COLOR_OFF "\n--------------------\n\n", delay);
    } else {
        printf(COLOR_RED "Error: invalid relay server name" COLOR_OFF "\n--------------------");
        return false;
    }

    return true;
}

bool connect_relay_multihop(char *relay_name_entry, char *relay_name_exit, int delay) {
    //enable multihop
    bool enable_multihop = apply_mullvad_relay("mullvad relay set multihop off", "");

    if (!enable_multihop) {
        printf(COLOR_RED "Error: unable to enable multihop mode\n" COLOR_OFF);
        return false;
    }

    bool apply_entry = apply_mullvad_relay("mullvad relay set entry location %s 2>&1", relay_name_entry);
    bool apply_exit = apply_mullvad_relay("mullvad relay set location %s 2>&1", relay_name_exit);

    if (apply_entry && apply_exit) {
        //connect to mullvad
        printf(COLOR_GREEN "Relay server change completed successfully\n" COLOR_OFF);
        system("mullvad connect");
        printf("Switching relay in " COLOR_BOLD "%d secondes..." COLOR_OFF "\n--------------------\n\n", delay);
    } else {
        printf(COLOR_RED "Error: invalid relay server name" COLOR_OFF "\n--------------------");
        return false;
    }

    return true;
}

void connect_random_relay(int delay, bool only_owned, char *country_tag, bool multihop_enabled) {
    int nb_relays = 0;

    //get the list of mullvad relay servers and store them in an array
    char *relay_list = get_relays_info();
    char **matchs = get_array_relays_list(relay_list, &nb_relays, only_owned, country_tag);

    if (nb_relays == 0 || matchs == NULL) {
        printf("Error getting relays list\n");
        free(relay_list);
        free_array_of_strings(matchs, nb_relays);
        return;
    }

    char suffix[128] = {0};
    if (only_owned && country_tag[0] != '\0') {
        snprintf(suffix, sizeof(suffix), " (owned by Mullvad and corresponding to country tag \"%s\")", country_tag);
    } else if (only_owned) {
        snprintf(suffix, sizeof(suffix), " (owned by Mullvad)");
    } else if (country_tag[0] != '\0') {
        snprintf(suffix, sizeof(suffix), " (corresponding to country tag \"%s\")", country_tag);
    }

    srand(time(NULL));

    do {
        if (multihop_enabled) {
            //get a random number for the entry server
            int random_number_1 = (rand() % (nb_relays));
            //get a random number for the exit server
            int random_number_2;

            do {
                random_number_2 = (rand() % (nb_relays));
            } while (random_number_1 == random_number_2);

            printf("--------------------\nRandom " COLOR_BOLD "entry" COLOR_OFF " relay picked: " COLOR_BOLD COLOR_GREEN "%s" COLOR_OFF "%s\n", matchs[random_number_1], suffix);
            printf("Random " COLOR_BOLD "exit " COLOR_OFF "relay picked: " COLOR_BOLD COLOR_GREEN "%s" COLOR_OFF "%s\n--------------------\n", matchs[random_number_2], suffix);
            
            if (!connect_relay_multihop(matchs[random_number_1], matchs[random_number_2], delay)) {
                break;
            }

        } else {
            //get a random relay server
            int random_number = (rand() % (nb_relays));
            printf("--------------------\nRandom entry relay picked: " COLOR_BOLD COLOR_GREEN "%s" COLOR_OFF "%s\n--------------------\n", matchs[random_number], suffix);

            if (!connect_relay(matchs[random_number], delay)) {
                break;
            }
        }
        
        SLEEP_SECONDS(delay);
    } while (true);

    
    free(relay_list);
    free_array_of_strings(matchs, nb_relays);
}

void connect_specific_relay(int delay, char **relay_names, int nb_relays, bool multihop_enabled) {
    if (multihop_enabled && nb_relays < 2) {
        printf(COLOR_RED "Error: Not enough relays found for multihop (found %d, need at least 2)\n" COLOR_OFF, nb_relays);
        return;
    }

    srand(time(NULL));

    do {
        if (multihop_enabled) {
            //get a random number for the entry server
            int random_number_1 = (rand() % (nb_relays));
            //get a random number for the exit server
            int random_number_2;

            do {
                random_number_2 = (rand() % (nb_relays));
            } while (random_number_1 == random_number_2);

            printf("--------------------\nRandom entry relay picked: " COLOR_BOLD COLOR_GREEN "%s" COLOR_OFF "\n", relay_names[random_number_1]);
            printf("Random exit relay picked: " COLOR_BOLD COLOR_GREEN "%s" COLOR_OFF "\n--------------------\n", relay_names[random_number_2]);
            
            if (!connect_relay_multihop(relay_names[random_number_1], relay_names[random_number_2], delay)) {
                break;
            }

        } else {
            //get a random relay server
            int random_number = (rand() % (nb_relays));
            printf("--------------------\nRandom entry relay picked: " COLOR_BOLD COLOR_GREEN "%s" COLOR_OFF "\n--------------------\n", relay_names[random_number]);

            if (!connect_relay(relay_names[random_number], delay)) {
                break;
            }
        }

        SLEEP_SECONDS(delay);
    } while (true);
}