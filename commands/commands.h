#ifndef COMMANDS_H
#define COMMANDS_H

int get_relay_count(bool only_owned, char *country_tag, char *city_tag);
void print_relays_list_formatted(bool only_owned, char *country_tag, char *city_tag);
void connect_random_relay(int delay, bool only_owned, char *country_tag, char *city_tag, bool multihop_enabled);
void connect_specific_relay(int delay, char **relay_names, int nb_relays, bool multihop_enabled);
bool connect_relay(char *relay_name, int delay);
bool connect_relay_multihop(char *relay_name_entry, char *relay_name_exit, int delay);


#endif