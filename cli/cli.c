#include <stdio.h>

#include "../utils/utils.h"

void print_options() {
    printf("\n\nOptions:");
    printf("\n%2s%-30s%s", "", COLOR_BOLD "--only-owned" COLOR_OFF, "Only use relays owned by Mullvad (excludes rented servers)");
    printf("\n%2s%-30s%s", "", COLOR_BOLD "--only-<COUNTRY-TAG>" COLOR_OFF, "Only use relays from a specific country (<COUNTRY-TAG>: the first 2 letters of each relay server name, examples: fr,no,se...)");
    printf("\n%2s%-30s%s", "", COLOR_BOLD "--multihop" COLOR_OFF, "Enabling multihop (routing traffic through an entry relay and then an exit relay) works the same way, the two relays are chosen at random.");
}

void print_default_usage() {
    printf("Usage: mullvad_rotator <COMMAND> [SUBCOMMAND] [OPTIONS]");
    printf("\n\nCommands:");
    printf("\n%2s%-30s%s", "", COLOR_BOLD "help" COLOR_OFF, "Display this helper");
    printf("\n%2s%-30s%s", "", COLOR_BOLD "relay" COLOR_OFF " <COMMAND>", "Count/list relays available for connection (<COMMAND>: count, list)");
    printf("\n%6s%-26s%s", "", COLOR_BOLD "count" COLOR_OFF, "Display the number of relays available for connection");
    printf("\n%6s%-26s%s", "", COLOR_BOLD "list" COLOR_OFF, "Display the list of all relays available for connection");
    printf("\n%2s%-30s%s", "", COLOR_BOLD "connect" COLOR_OFF " <random|NAMES>", "Connect to a Mullvad relay server");
    printf("\n%6s%-26s%s", "", COLOR_BOLD "random" COLOR_OFF, "Connect to a random relay server");
    printf("\n%6s%-26s%s", "", COLOR_BOLD "<NAMES>" COLOR_OFF, "Connect to one or more specific relay servers (separated by commas, with no space, examples of valid relay names: nl-ams-wg-007,no-osl-wg-002,se-sto-wg-014...)");
    printf("\n%6s%-26s%s", "", COLOR_BOLD "-t" COLOR_OFF " <SECONDS>", "Specify the number of seconds for the server rotation (default: 120 seconds)");
    print_options();
}