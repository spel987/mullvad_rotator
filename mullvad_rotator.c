#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2posix.h> //because im using windows, i can't use "regex.h" but if you're using linux/mac, replace it with "regex.h"
#include <time.h>
#include <stdbool.h>
#include <windows.h>

#define COLOR_BOLD "\e[1m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_OFF "\e[m"

char *get_relays_info();
char **get_array_relays_list(char *relay_list, int *nb_relays, bool only_owned);
int get_relay_count(bool only_owned);
void print_relays_list_formatted(bool only_owned);
void connect_random_relay(int delay, bool only_owned);
void free_array_of_strings(char **array, int nb_elements);
bool digit_check(char *key);
void print_default_usage();

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
        array_commands = malloc((argc - 1) * sizeof(char*));
    } else {
        array_commands = malloc((argc) * sizeof(char*));
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

    if (strcmp(array_commands[1], "relay") == 0) {
        if (array_commands[2] != NULL && strcmp(array_commands[2], "count") == 0) {
            printf("There are currently %d relays available for connection!\n", get_relay_count(only_owned));
        } else if (array_commands[2] != NULL && strcmp(array_commands[2], "list") == 0) {
            print_relays_list_formatted(only_owned);
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

char *get_relays_info() {
    //create buffer of 1024 bytes
    int capacity = 1024;
    int size = 0;

    char *buffer = malloc(capacity * sizeof(char));

    if (buffer == NULL) {
        return NULL;
    }

    //run the command to display all Mullvad relay servers
    FILE *cmd = popen("mullvad relay list", "r");

    if (cmd == NULL) {
        free(buffer);
        return NULL;
    }

    //get the content of the command's output
    while (1) {
        //if there is no more space in our buffer, we have to make it bigger
        if (size == capacity - 1) {
            capacity *= 2;
            char *temp = realloc(buffer, capacity * sizeof(char));

            if (temp == NULL) {
                buffer[size] = '\0';
                break;
            }
            buffer = temp;           
        }

        //reading the content with fread
        int empty_space = capacity - size - 1;
        int nb_read = fread(buffer + size, sizeof(char), empty_space, cmd);
        size += nb_read;

        //at the end of the content, add '\0' to properly end the string
        if (nb_read == 0) {
            buffer[size] = '\0';
            break;
        }        
    }

    //closing the "file" = the content of the command's output
    int value_pclose = pclose(cmd);
    
    if (value_pclose != 0) {
        perror("Error when closing result content of the cmd");
    }

    return buffer;
}

char **get_array_relays_list(char *relay_list, int *nb_relays, bool only_owned) {   
    regex_t re;
    regmatch_t match[3];

    char* cursor = relay_list;
    int nb_result = 0;

    //pattern detection of Mullvad relay server names
    const char *pattern = "([a-z]+-[a-z]+-wg-[0-9]+) \\([0-9.]+, [a-z0-9:]+\\) - hosted by [a-zA-Z0-9]+ \\(([a-zA-Z0-9-]+)\\)";

    //create an array that will contains strings
    int capacity = 1024;
    int size = 0;
    char **results = malloc(capacity * sizeof(char*));

    if (results == NULL) {
        return NULL;
    }

    //compile regex
    if (regcomp(&re, pattern, REG_EXTENDED)) {
        perror("Regex compile failed");
        return NULL;
    }

    //execute regex
    while(regexec(&re, cursor, 3, match, 0) == 0 && nb_result < 1000) {
        //create a new buffer for the type (rented/owned) of the relay
        int type_len = match[2].rm_eo - match[2].rm_so;
        char *type = malloc(type_len + 1);
    
        strncpy(type, cursor + match[2].rm_so, type_len);

        type[type_len] = '\0';

        //if we are in the "only owned" mode, we skip the rented ones
        if (only_owned && strcmp(type, "Mullvad-owned") != 0) {
            cursor += match[0].rm_eo;
            continue;
        }

        //if there is no more space in our array of strings containing the results, we have to make it bigger
        if (size == capacity - 1) {
            capacity *= 2;
            char **temp = realloc(results, capacity * sizeof(char*));

            if (temp == NULL) {
                results[size] = '\0';
                break;
            }
            results = temp;           
        }

        //create a new buffer for the name of the relay
        int name_len = match[1].rm_eo - match[1].rm_so;
        char *result_i = malloc(name_len + 1);

        strncpy(result_i, cursor + match[1].rm_so, name_len);

        result_i[name_len] = '\0';
        
        //add the match to the arrays
        results[size] = result_i;

        //move the cursor to detect the following relays
        cursor += match[2].rm_eo;
        nb_result++;
        size++;
    }

    //cleaning
    regfree(&re);

    //putting in the "nb_relays" var the actual number of relays that match our regex
    *nb_relays = size;

    return results;
}

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

        Sleep(delay * 1000);
    } while (is_running);

    
    free(relay_list);
    free_array_of_strings(matchs, nb_relays);
}

void free_array_of_strings(char **array, int nb_elements) {
    //free each element of the array
    for (int i = 0; i < nb_elements; i++) {
        free(array[i]);
    }

    //free the array
    free(array);
}

bool digit_check(char *key) {
    int i = 0;

    while (i < strlen(key) && isdigit(key[i])) {
        i++;
    }

    return i == strlen(key);
}

void print_default_usage() {
    printf("Usage: mullvad_rotator <COMMAND> [SUBCOMMAND] [OPTIONS]");
    printf("\n\nCommands:");
    printf("\n%2s%-30s%s", "", COLOR_BOLD "relay" COLOR_OFF " <COMMAND>", "Count/list relays available for connection (<COMMAND>: count, list)");
    printf("\n%6s%-26s%s", "", COLOR_BOLD "count" COLOR_OFF, "Display the number of relays available for connection");
    printf("\n%6s%-26s%s", "", COLOR_BOLD "list" COLOR_OFF, "Display the list of all relays available for connection");
    printf("\n%2s%-30s%s", "", COLOR_BOLD "connect" COLOR_OFF " <COMMAND>", "Connect to a Mullvad relay server (<COMMAND>: random)");
    printf("\n%6s%-26s%s", "", COLOR_BOLD "random" COLOR_OFF, "Connect to a random relay server");
    printf("\n%10s%-22s%s", "", COLOR_BOLD "-t" COLOR_OFF " <SECONDS>", "Specify the number of seconds for the server rotation (default: 120 seconds)");
    printf("\n\nOptions:");
    printf("\n%2s%-30s%s", "", COLOR_BOLD "--only-owned" COLOR_OFF, "Only use relays owned by Mullvad (excludes rented servers)");
}