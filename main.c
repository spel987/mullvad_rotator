#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>

#include "cli/cli.h"
#include "commands/commands.h"
#include "utils/utils.h"
#include "relay_data/relay_data.h"
#include "platform/platform.h"

int main(int argc, char **argv) {
    if (!check_mullvad_availability()) {
        printf(COLOR_RED "Error: Mullvad was not found on your computer. Please install it first.\n" COLOR_OFF);
        return EXIT_FAILURE;
    }

    if (!check_mullvad_account()) {
        printf(COLOR_RED "Error: No account associated with Mullvad was detected. Please log in first.\n" COLOR_OFF);
        return EXIT_FAILURE;
    }
    
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);

    if (argc < 2) {
        print_default_usage();
        return EXIT_FAILURE;
    }

    //boolean for the option "--only-owned"
    bool only_owned = false;
    //string for the country tag (if specified, null by default)
    char *country_tag = calloc(3, sizeof(char));
    //string for the city tag (if specified, null by default)
    char *city_tag = calloc(4, sizeof(char));
    //boolean for the option "--multihop"
    bool multihop_enabled = false;

    regex_t re;
    regmatch_t match_country_city_tag[3];
    //pattern of the country tag and the city tag (optional)
    const char *pattern_country_city_tag = "--only-([a-z]{2})(?:-([a-z]{3}))?";
    regcomp(&re, pattern_country_city_tag, REG_EXTENDED);

    //arrays of the parameters with options such as "--only-owned" and "--only-<COUNTRY-TAG>" removed
    char **array_commands = malloc(argc * sizeof(char*));

    int nb_commands = 0;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--only-owned") == 0) {
            only_owned = true;
        } else if (regexec(&re, argv[i], 3, match_country_city_tag, 0) == 0) {
            strncpy(country_tag, argv[i] + match_country_city_tag[1].rm_so, 2);
            country_tag[2] = '\0';

            //if a city tag is specified
            if (match_country_city_tag[2].rm_so != -1) {
                strncpy(city_tag, argv[i] + match_country_city_tag[2].rm_so, 3);
                city_tag[3] = '\0';
            }
            
        } else if (strcmp(argv[i], "--multihop") == 0) {
            multihop_enabled = true;
        } else {
            char *element_i = argv[i];
            array_commands[nb_commands] = element_i;
            nb_commands++;
        }
    }

    array_commands[nb_commands] = NULL;

    //help
    if (strcmp(array_commands[1], "help") == 0) {
        print_default_usage();
        return 0;

    //relay
    } else if (strcmp(array_commands[1], "relay") == 0) {
        //relay count
        if (array_commands[2] != NULL && strcmp(array_commands[2], "count") == 0) {
            printf("There are currently %d relays available for connection!\n", get_relay_count(only_owned, country_tag, city_tag));
            return 0;
        //relay list
        } else if (array_commands[2] != NULL && strcmp(array_commands[2], "list") == 0) {
            print_relays_list_formatted(only_owned, country_tag, city_tag);
            return 0;
        } else {
            printf("Usage: mullvad_rotator relay <SUBCOMMANDS> [OPTIONS]");
            printf("\n\nSubcommands:");
            printf("\n%2s%-34s%s", "", COLOR_BOLD "count" COLOR_OFF, "Display the number of relays available for connection");
            printf("\n%2s%-34s%s", "", COLOR_BOLD "list" COLOR_OFF, "Display the list of all relays available for connection");
            print_options();
            return EXIT_FAILURE;
        }

    //connect
    } else if (strcmp(array_commands[1], "connect") == 0) {
        //connect random
        //connect to a random relay with default rotation time
        if (array_commands[2] != NULL && strcmp(array_commands[2], "random") == 0 && array_commands[3] == NULL) {
            connect_random_relay(120, only_owned, country_tag, city_tag, multihop_enabled);
        
        //connect random -t X
        //connect to a random relay with a custom rotation time
        } else if (array_commands[2] != NULL && strcmp(array_commands[2], "random") == 0 && strcmp(array_commands[3], "-t") == 0 && array_commands[4] != NULL) {
            if (!digit_check(array_commands[4])) {
                printf(COLOR_RED "Error: not a valid number\n\n" COLOR_OFF);

                printf("Usage: mullvad_rotator connect random [OPTIONS]");
                printf("\n\nSubcommands:");
                printf("\n%2s%-34s%s", "", COLOR_BOLD "-t" COLOR_OFF " <SECONDS>", "Specify the number of seconds for the server rotation (default: 120 seconds)");
                print_options();
                return EXIT_FAILURE;
            } else {
                connect_random_relay(atoi(array_commands[4]), only_owned, country_tag, city_tag, multihop_enabled);
            }
        
        //connect RELAY_NAME1,RELAY_NAME2,RELAY_NAME3
        } else if (array_commands[2] != NULL && array_commands[3] == NULL) {
            int nb_relays = 0;
            char **relay_names = parse_relay_names(array_commands[2], &nb_relays);

            if (relay_names == NULL) {
                return EXIT_FAILURE;
            }

            connect_specific_relay(120, relay_names, nb_relays, multihop_enabled);

        //connect RELAY_NAME1,RELAY_NAME2,RELAY_NAME3 -t X
        } else if (array_commands[2] != NULL && strcmp(array_commands[3], "-t") == 0 && array_commands[4] != NULL) {
            if (!digit_check(array_commands[4])) {
                printf(COLOR_RED "Error: not a valid number\n\n" COLOR_OFF);
                printf("Usage: mullvad_rotator connect <NAMES> [OPTIONS]");
                printf("\n\nSubcommands:");
                printf("\n%2s%-34s%s", "", COLOR_BOLD "-t" COLOR_OFF " <SECONDS>", "Specify the number of seconds for the server rotation (default: 120 seconds)");
                return EXIT_FAILURE;
            } else {
                int nb_relays = 0;
                char **relay_names = parse_relay_names(array_commands[2], &nb_relays);

                if (relay_names == NULL) {
                    return EXIT_FAILURE;
                }

                connect_specific_relay(atoi(array_commands[4]), relay_names, nb_relays, multihop_enabled);
            }

        } else {
            printf("Usage: mullvad_rotator connect <SUBCOMMANDS> [OPTIONS]");
            printf("\n\nSubcommands:");
            printf("\n%2s%-34s%s", "", COLOR_BOLD "random" COLOR_OFF, "Connect to a random relay server");
            printf("\n%2s%-34s%s", "", COLOR_BOLD "<NAMES>" COLOR_OFF, "Connect to one or more specific relay servers (separated by commas, with no space, examples of valid relay names: nl-ams-wg-007,no-osl-wg-002,se-sto-wg-014)");
            printf("\n%2s%-34s%s", "", COLOR_BOLD "-t" COLOR_OFF " <SECONDS>", "Specify the number of seconds for the server rotation (default: 120 seconds)");
            print_options();
            return EXIT_FAILURE;
        }
    } else {
        print_default_usage();
        return EXIT_FAILURE;
    }

    return 0;
}