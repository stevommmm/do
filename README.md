# ok

Experiment in programming a programming language without all the *features* of other tools.

Think of the script files as dumb sequential if statements.


| KEYWORD      | USAGE   |
|--------------|---------|
| `SYNC` <url> | Use libcurl to fetch a remote script & exec |
| `IF` <cmd>   | A command with a zero exit code |
| `NIF` <cmd>  | A command resulting in a non-zero exit code |
| `DO` <cmd>   | A command to execute, optionally if the above `[N]IF` met the criteria |


todo

- [x] handle superfluous indenting characters
- [x] parse nested indenting
- [x] fetch remote scripts


### Example usage:
```text
DO echo "beginning!"

NIF rpm -q iptables
    DO yum install iptables -y

NIF iptables -C INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
    DO iptables -A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT

NIF iptables -C INPUT -i lo -j ACCEPT
    DO iptables -A INPUT -i lo -j ACCEPT

SYNC http://localhost/test.f

```

```bash
echo -e 'IF true\n\techo it must be so!' > test.f
make
./ok ./test.f
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
