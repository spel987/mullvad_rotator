#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2posix.h> //because im on windows i cant use "regex.h" but if you'r on linux/mac replace it by "regex.h"
#include <time.h>

char *get_relays_info();
char **get_array_relays_list(char *relay_list, int *nb_relays);
int get_relay_count();
void print_relays_list_formatted();
void connect_random_relay();
void free_array_of_strings(char **array, int nb_elements);

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: mullvad_rotator <COMMAND>");
        printf("\n\nCommands:\n");
        printf("\n  %-20s%s", "relay_count", "Display the number of relays available for connection");
        printf("\n  %-20s%s", "relay_list", "Display the list of all relays available for connection");
        printf("\n  %-20s%s", "connect <COMMAND>", "Connect to a Mullvad relay server (<COMMAND>: random)");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "relay_count") == 0) {
        printf("There are currently %d relays available for connection!\n", get_relay_count());

    } else if (strcmp(argv[1], "relay_list") == 0) {
        print_relays_list_formatted();

    } else if (strcmp(argv[1], "connect") == 0) {
        if (argv[2] != NULL && strcmp(argv[2], "random") == 0) {
            connect_random_relay();
        } else {
            printf("Usage: mullvad_rotator connect <COMMAND>");
            printf("\n\nCommands:\n");
            printf("\n  %-20s%s\n", "random", "Connect to a random Mullvad relay server");
        }
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

    //execute the command
    FILE *cmd = popen("mullvad relay list", "r");

    if (cmd == NULL) {
        free(buffer);
        return NULL;
    }

    //get result-content of the command
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

        //at the end of the content, adding '\0' to properly end the string
        if (nb_read == 0) {
            buffer[size] = '\0';
            break;
        }        
    }

    //closing the "file" = the result content of the command
    int value_pclose = pclose(cmd);
    
    if (value_pclose != 0) {
        printf("Error when closing result content of the cmd");
    }

    return buffer;
}

char **get_array_relays_list(char *relay_list, int *nb_relays) {
    regex_t re;
    regmatch_t match[1];
    char* cursor = relay_list;
    int nb_result = 0;

    const char *pattern = "([a-z]+)-([a-z]+)-wg-([0-9]+)";

    //creating an array that will contains string
    int capacity = 1024;
    int size = 0;

    char **results = malloc(capacity * sizeof(char*));

    if (results == NULL) {
        return NULL;
    }

    //compiling regex
    if (regcomp(&re, pattern, REG_EXTENDED)) {
        perror("regex compile failed");
        return NULL;
    }

    //execute
    while(regexec(&re, cursor, 1, match, 0) == 0 && nb_result < 1000) {
        //if there is no more space in our result array of strings, we have to make it bigger
        if (size == capacity - 1) {
            capacity *= 2;
            char **temp = realloc(results, capacity * sizeof(char*));

            if (temp == NULL) {
                results[size] = '\0';
                break;
            }
            results = temp;           
        }

        char *result_i = malloc((int)(match[0].rm_eo - match[0].rm_so) + 1);
        strncpy(result_i, cursor + match[0].rm_so, (int)(match[0].rm_eo - match[0].rm_so));
        result_i[(int)(match[0].rm_eo - match[0].rm_so)] = '\0';

        results[size] = result_i;

        cursor += match[0].rm_eo;
        nb_result++;
        size++;
    }

    //cleaning
    regfree(&re);

    //putting in the "nb_relays" var the actual number of relays that match our regex
    *nb_relays = size;

    return results;
}

int get_relay_count() {
    int nb_relays = 0;

    char *relay_list = get_relays_info();
    char **matchs = get_array_relays_list(relay_list, &nb_relays);

    free(relay_list);
    free_array_of_strings(matchs, nb_relays);

    return nb_relays;
}

void print_relays_list_formatted() {
    int nb_relays = 0;
    
    char *relay_list = get_relays_info();
    char **matchs = get_array_relays_list(relay_list, &nb_relays);

    for (int i = 0; i < nb_relays; i++) {
        printf("%s\n", matchs[i]);
    }

    free(relay_list);
    free_array_of_strings(matchs, nb_relays);
}

void connect_random_relay() {
    int nb_relays = 0;

    char *relay_list = get_relays_info();
    char **matchs = get_array_relays_list(relay_list, &nb_relays);

    srand(time(NULL));
    int random_number = (rand() % (nb_relays));
    printf("random relay picked: %s\n", matchs[random_number]);

    int cmd_len = strlen("mullvad relay set location ") + strlen(matchs[random_number]);

    char *relay_set_cmd = malloc(cmd_len);
    snprintf(relay_set_cmd, cmd_len, "mullvad relay set location %s", matchs[random_number]);

    system(relay_set_cmd);

    free(relay_set_cmd);
    free(relay_list);
    free_array_of_strings(matchs, nb_relays);

    system("mullvad connect");
}

void free_array_of_strings(char **array, int nb_elements) {
    for (int i = 0; i < nb_elements; i++) {
        free(array[i]);
    }

    free(array);
}