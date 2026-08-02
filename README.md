# Mullvad rotator

Connect to a randomly selected Mullvad relay server and automatically switch to another server every X seconds.

```
Usage: mullvad_rotator <COMMAND> [SUBCOMMAND] [OPTIONS]

Commands:
  help                   Display this helper
  relay <COMMAND>        Count/list relays available for connection (<COMMAND>: count, list)
      count              Display the number of relays available for connection
      list               Display the list of all relays available for connection
  connect <COMMAND>      Connect to a Mullvad relay server (<COMMAND>: random)
      random             Connect to a random relay server
          -t <SECONDS>   Specify the number of seconds for the server rotation (default: 120 seconds)

Options:
  --only-owned           Only use relays owned by Mullvad (excludes rented servers)
```

## Prerequisites
- Have Mullvad installed
- Be logged in to a Mullvad account

## Examples of commands
### Connect to a random relay and switch relays every 120 seconds (default)
```
./mullvad_rotator connect random
```
### Connect to a random relay and switch relays every 30 seconds (custom)
```
./mullvad_rotator connect random -t 30
```
### Connect to a random relay owned by Mullvad and switch relays every 10 seconds (custom)
```
./mullvad_rotator connect random -t 30 --owned-only
```

## Note

This is the first C project I've worked on. It's more of a practice exercise than a real project.
I plan to improve it in the future.

<img width=100 src="https://i.imgur.com/O1I2BYJ.png">