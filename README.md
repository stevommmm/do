# do

Get things done.

Experiment in programming a programming language without all the *features* of other tools.

Think of the script files as dumb sequential if statements.


| KEYWORD             | USAGE   |
|---------------------|---------|
| `SYNC <url>`        | Use libcurl to fetch a remote script & exec |
| `IF <cmd> ...`      | A command with a zero exit code |
| `NOT`               | Negates the previous IF to a non-zero exit code |
| `DO <cmd> ...`      | A command to execute, optionally if the above `[N]IF` met the criteria |
| `SET %VAR% <str>`   | Create a variable to the contents of the next node, all later occurances are replaced. |
| `NRDP <nagios> <token> <service> <hostname> <state:int> <output>` | Send passive data via Nagios Remote Data Processor. |

todo

- [x] handle superfluous indenting characters
- [x] parse nested indenting
- [x] fetch remote scripts
- [x] variables / facts
- [ ] parse nagios state *OK|WARN|CRIT* to ints for NRDP
- [ ] global prefil `%FQDN%` variable


### Example usage:
```text
SET %MODES% "RELATED,ESTABLISHED"

DO echo "beginning!"

IF NOT rpm -q iptables
    DO yum install iptables -y

IF NOT iptables -C INPUT -m state --state %MODES% -j ACCEPT
    DO iptables -A INPUT -m state --state %MODES% -j ACCEPT

IF NOT iptables -C INPUT -i lo -j ACCEPT
    DO iptables -A INPUT -i lo -j ACCEPT

SYNC http://localhost/test.do

```

```bash
echo -e 'IF true\n\tDO echo it must be so!' > test.do
make
./do-run ./test.do
```


# Building

ubuntu:
```bash
apt install -y make gcc libcurl4-openssl-dev --no-install-recommends
make
```

centos:
```bash
yum install -y make gcc libcurl-devel
make
```

arm (from x86):
```bash
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
docker run --rm -ti -v $(pwd):/mnt arm64v8/centos /bin/bash
# follow centos build process
```
