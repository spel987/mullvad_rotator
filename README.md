# Mullvad rotator

### Overview
- Connect to a random Mullvad relay server and switch every X seconds
- Connect to predefined Mullvad relay servers and switch every X seconds
- Filter only servers owned by Mullvad
- Filter only servers located in a specific country

```
Usage: mullvad_rotator <COMMAND> [SUBCOMMAND] [OPTIONS]

Commands:
  help                   Display this helper
  relay <COMMAND>        Count/list relays available for connection (<COMMAND>: count, list)
      count              Display the number of relays available for connection
      list               Display the list of all relays available for connection
  connect <random|NAMES> Connect to a Mullvad relay server
      random             Connect to a random relay server
      <NAMES>            Connect to one or more specific relay servers (separated by commas, with no space, examples of valid relay names: nl-ams-wg-007,no-osl-wg-002,se-sto-wg-014...)
      -t <SECONDS>       Specify the number of seconds for the server rotation (default: 120 seconds)

Options:
  --only-owned           Only use relays owned by Mullvad (excludes rented servers)
  --only-<COUNTRY-TAG>   Only use relays from a specific country (<COUNTRY-TAG>: the first 2 letters of each relay server name, examples: fr,no,se...)
```

## Prerequisites
- Have Mullvad installed
- Be logged in to a Mullvad account

**Warning**: There isn't really any error handling right now if Mullvad isn't installed. So make sure you have it on your machine if you want this to work. As I mention below, the script will be improved in the future.

## Examples of commands
### Connect to a random relay and switch relays every 120 seconds (default)
```
./mullvad_rotator connect random
```
### Connect to a random relay and switch relays every 30 seconds (custom)
```
./mullvad_rotator connect random -t 30
```
### Connect to a random relay owned by Mullvad and switch relays every 20 seconds (custom)
```
./mullvad_rotator connect random -t 20 --owned-only
```
### Connect to 3 specific relays and switch relays every 40 seconds (custom)
```
./mullvad_rotator connect fr-par-wg-101,se-mma-wg-011,jp-tyo-wg-202 -t 20
```
### Connect to a random relay located in Sweden owned by Mullvad and switch relays every 5 seconds (custom)
```
./mullvad_rotator connect random -t 5 --only-owned --only-se
```

## Note

This is the first C project I've worked on. Think of it as an exercise, but if it's useful to you, I'll be happy.
I plan to improve it in the future.

<img width=100 src="https://i.imgur.com/O1I2BYJ.png">