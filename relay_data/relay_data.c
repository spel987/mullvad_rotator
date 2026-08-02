#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../platform/platform.h"
#include "../utils/utils.h"

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
    while (true) {
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

char **get_array_relays_list(char *relay_list, int *nb_relays, bool only_owned, char *country_tag) {   
    regex_t re;
    regmatch_t match[4];

    char* cursor = relay_list;
    int nb_result = 0;

    //pattern detection of Mullvad relay server names
    const char *pattern = "(([a-z]+)-[a-z]+-wg-[0-9]+) \\([0-9.]+, [a-z0-9:]+\\) - hosted by [a-zA-Z0-9]+ \\(([a-zA-Z0-9-]+)\\)";

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
    while(regexec(&re, cursor, 4, match, 0) == 0 && nb_result < 1000) {
        //create a new buffer for the type (rented/owned) of the relay
        int type_len = match[3].rm_eo - match[3].rm_so;
        char *type = malloc(type_len + 1);
    
        strncpy(type, cursor + match[3].rm_so, type_len);

        type[type_len] = '\0';

        //if the option "only-owned" is enable, relay servers that are not owned by Mullvad are ignored
        if (only_owned && strcmp(type, "Mullvad-owned") != 0) {
            cursor += match[0].rm_eo;
            free(type);
            continue;
        }

        free(type);

        //if a country tag is specified, tags that do not apply to that country are ignored
        if (country_tag[0] != '\0' && strncmp(cursor + match[2].rm_so, country_tag, 2)) {
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
        cursor += match[3].rm_eo;
        nb_result++;
        size++;
    }

    //cleaning
    regfree(&re);

    //putting in the "nb_relays" var the actual number of relays that match our regex
    *nb_relays = size;

    return results;
}

void free_array_of_strings(char **array, int nb_elements) {
    //free each element of the array
    for (int i = 0; i < nb_elements; i++) {
        free(array[i]);
    }

    //free the array
    free(array);
}

char **parse_relay_names(char *input, int *nb_relays) {
    int capacity = 32;
    int size = 0;

    char **relay_names = malloc(capacity * sizeof(char*));

    char *token = strtok(input, ",");

    while (token != NULL) {
        regex_t re;
        regmatch_t match[1];
        const char *pattern = "[a-z]+-[a-z]+-wg-[0-9]+";

        //compile regex
        if (regcomp(&re, pattern, REG_EXTENDED)) {
            perror("Regex compile failed");
            return NULL;
        }
        
        //if the relay name doesnt match with the pattern
        if (regexec(&re, token, 1, match, 0) != 0) {
            printf(COLOR_RED "Error: invalid format for relay names\n" COLOR_OFF);
            printf("\nExamples of valid relay names: " COLOR_BOLD "nl-ams-wg-007,no-osl-wg-002,se-sto-wg-014" COLOR_OFF);
            printf("\n\nUsage: mullvad_rotator connect <NAMES> [OPTIONS]");
            printf("\n\nSubcommands:");
            printf("\n%2s%-30s%s", "", COLOR_BOLD "-t" COLOR_OFF " <SECONDS>", "Specify the number of seconds for the server rotation (default: 120 seconds)");

            return NULL;
        }

        if (size == capacity - 1) {
            capacity *= 2;
            char **temp = realloc(relay_names, capacity * sizeof(char*));
            
            if (temp == NULL) {
                relay_names[size] = '\0';
                break;
            }

            relay_names = temp;
        }

        int name_len = strlen(token);

        char *result_i = malloc(name_len + 1);

        strncpy(result_i, token, name_len);

        result_i[name_len] = '\0';

        relay_names[size] = result_i;

        token = strtok(NULL, ",");
        size++;
    }

    *nb_relays = size;

    return relay_names;
}