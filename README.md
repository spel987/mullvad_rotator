# Mullvad rotator

Connect to a randomly selected Mullvad relay server and automatically switch to another server every X seconds.

```
Usage: mullvad_rotator <COMMAND>

Commands:

  relay_count         Display the number of relays available for connection
  relay_list          Display the list of all relays available for connection
  connect <COMMAND>   Connect to a Mullvad relay server (<COMMAND>: random)
```

## Examples of commands
### Connect to a random relay and switch relays every 120 seconds (default)
```
./mullvad_rotator connect random
```
### Connect to a random relay and switch relays every 30 seconds (custom)
```
./mullvad_rotator connect random -t 30
```

## Note

This is the first C project I've worked on. It's more of a practice exercise than a real project.
I plan to improve it in the future.

<img width=100 src="https://i.imgur.com/O1I2BYJ.png">