#ifndef COMMANDS_H
#define COMMANDS_H

int get_relay_count(bool only_owned);
void print_relays_list_formatted(bool only_owned);
void connect_random_relay(int delay, bool only_owned);
void connect_specific_relay(int delay, char **relay_names, int nb_relays);
bool connect_relay(char *relay_name, int delay);

#endif