# ok

Experiment in programming a programming language without all the *features* of other tools.

Think of the script files as dumb sequential if statements.


| KEYWORD      | USAGE   |
|--------------|---------|
| `SYNC` <url> | Use libcurl to fetch a remote script & exec |
| `IF` <cmd>   | A command with a zero exit code |
| `NIF` <cmd>  | A command resulting in a non-zero exit code |
| `\t`         | A command to execute if the above `[N]IF` met the criteria |


todo

- [x] handle superfluous indenting characters
- [x] parse nested indenting
- [x] fetch remote scripts


### Example usage:
```text
NIF test -f /etc/passwd
	echo "PANIC!"

NIF rpm -q iptables
    yum install iptables -y

NIF iptables -C INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
    iptables -A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT

NIF iptables -C INPUT -i lo -j ACCEPT
    iptables -A INPUT -i lo -j ACCEPT

SYNC http://localhost/test.f

```

```bash
echo -e 'IF true\n\techo it must be so!' > test.f
make
./ok ./test.f
```
