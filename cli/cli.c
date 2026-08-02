#include <stdio.h>

#include "../utils/utils.h"

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