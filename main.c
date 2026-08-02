#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cli/cli.h"
#include "commands/commands.h"
#include "utils/utils.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        print_default_usage();
        return EXIT_FAILURE;
    }

    bool only_owned = false;
    char **array_commands;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--only-owned") == 0) {
            only_owned = true;
        }
    }

    if (only_owned) {
        array_commands = malloc((argc) * sizeof(char*));
    } else {
        array_commands = malloc((argc + 1) * sizeof(char*));
    }

    int nb_commands = 0;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--only-owned") != 0) {
            char *element_i = argv[i];
            array_commands[nb_commands] = element_i;
            nb_commands++;
        }
    }

    array_commands[nb_commands] = NULL;

    if (strcmp(array_commands[1], "help") == 0) {
        print_default_usage();
        return 0;
    } else if (strcmp(array_commands[1], "relay") == 0) {
        if (array_commands[2] != NULL && strcmp(array_commands[2], "count") == 0) {
            printf("There are currently %d relays available for connection!\n", get_relay_count(only_owned));
            return 0;
        } else if (array_commands[2] != NULL && strcmp(array_commands[2], "list") == 0) {
            print_relays_list_formatted(only_owned);
            return 0;
        } else {
            printf("Usage: mullvad_rotator relay <SUBCOMMANDS> [OPTIONS]");
            printf("\n\nSubcommands:");
            printf("\n%2s%-30s%s", "", COLOR_BOLD "count" COLOR_OFF, "Display the number of relays available for connection");
            printf("\n%2s%-30s%s", "", COLOR_BOLD "list" COLOR_OFF, "Display the list of all relays available for connection");
            printf("\n\nOptions:");
            printf("\n%2s%-30s%s", "", COLOR_BOLD "--only-owned" COLOR_OFF, "Only use relays owned by Mullvad (excludes rented servers)");
            return EXIT_FAILURE;
        }

    } else if (strcmp(array_commands[1], "connect") == 0) {
        //connect to a random relay with default rotation time
        if (array_commands[2] != NULL && strcmp(array_commands[2], "random") == 0 && array_commands[3] == NULL) {
            connect_random_relay(120, only_owned);
        
        //connect to a random relay with a custom rotation time
        } else if (array_commands[2] != NULL && strcmp(array_commands[2], "random") == 0 && strcmp(array_commands[3], "-t") == 0 && array_commands[4] != NULL) {
            if (!digit_check(array_commands[4])) {
                printf(COLOR_RED "Error: not a valid number\n\n" COLOR_OFF);

                printf("Usage: mullvad_rotator connect random [OPTIONS]");
                printf("\n\nSubcommands:");
                printf("\n%2s%-30s%s", "", COLOR_BOLD "-t" COLOR_OFF " <SECONDS>", "Specify the number of seconds for the server rotation (default: 120 seconds)");
                printf("\n\nOptions:");
                printf("\n%2s%-30s%s", "", COLOR_BOLD "--only-owned" COLOR_OFF, "Only use relays owned by Mullvad (excludes rented servers)");
                return EXIT_FAILURE;
            } else {
                connect_random_relay(atoi(array_commands[4]), only_owned);
            }
        
        } else {
            printf("Usage: mullvad_rotator connect <SUBCOMMANDS> [OPTIONS]");
            printf("\n\nSubcommands:");
            printf("\n%2s%-30s%s", "", COLOR_BOLD "random" COLOR_OFF, "Connect to a random relay server");
            printf("\n%6s%-26s%s", "", COLOR_BOLD "-t" COLOR_OFF " <SECONDS>", "Specify the number of seconds for the server rotation (default: 120 seconds)");
            printf("\n\nOptions:");
            printf("\n%2s%-30s%s", "", COLOR_BOLD "--only-owned" COLOR_OFF, "Only use relays owned by Mullvad (excludes rented servers)");
            return EXIT_FAILURE;
        }
    } else {
        print_default_usage();
        return EXIT_FAILURE;
    }

    return 0;
}