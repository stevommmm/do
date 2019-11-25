# ok

Experiment in programming a programming language without all the *features* of other tools.

Think of the script files as dumb squential if statements.


| KEYWORD | USAGE   |
|---------|---------|
| `IF`    | A command with a zero exit code |
| `NIF`   | A command resuling in a non-zero exit code |
| `\t`    | A command to execute if the above `[N]IF` met the critera |

## todo

- [ ] handle superfluous indenting characters
- [ ] parse nested indenting


### Example usage:
```text
NIF test -f /etc/passwd
	echo "PANIC!"

NIF echo 'this has no "effect" as we have no conditional commands'

IF echo la la la la la
	echo "yes"
	echo "echo is always truthy"
	echo "we can indent so many times!"

NIF echo "	"

IF true
	echo "as expected /bin/true == 0"

IF false
	echo "WAT /bin/false == 0? dis solaris?"

NIF false
	echo "as expected /bin/false == 1"

```

```bash
mkdir -p scripts
echo 'IF echo meow' > scripts/test.f
make
./ok
```
