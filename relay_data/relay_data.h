#ifndef RELAY_DATA_H
#define RELAY_DATA_H

char *get_relays_info();
char **get_array_relays_list(char *relay_list, int *nb_relays, bool only_owned, char *country_tag, char *city_tag);
void free_array_of_strings(char **array, int nb_elements);
char **parse_relay_names(char *input, int *nb_relays);

#endif