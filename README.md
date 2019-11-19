# ok

Experiment in state management without all the *features* of other tools.

Follows an uncomplicated language for checking state and applying actions based on the inital character of the line.

Think of the script files as dumb if statements.


| SYMBOL | USAGE   |
|--------|---------|
| `#`    | Comment |
| `?`    | A command with a zero exit code |
| `!`    | A command resuling in a non-zero exit code |
| `\t`   | A command to execute if the above `?|!` met the critera |


Tiny example usage

```text
!pacman -Qi iptables
	pacman -Sq iptables

!iptables -C INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
	iptables -A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT

!echo 'this has no "effect" as we have no conditional commands'
?echo "la la la la la la la 'la'"
```

```bash
mkdir -p scripts
echo '!echo meow' > scripts/test.f
make
./ok
```
