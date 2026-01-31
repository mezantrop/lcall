# lcall

`lcall` is a minimalistic CLI utility for calling selected `libc` functions
directly from the command line.

It provides a lightweight bridge between shell scripts and POSIX APIs,
without using interpreters, high-level languages, or heavy runtime dependencies.

<a href="https://www.buymeacoffee.com/mezantrop" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"></a>

## Motivation

Sometimes you just need to:

* call a system function directly from a shell script
* query DNS or network information
* convert interface names ↔ indexes
* do all this without Python, Perl, or other scripting languages

`lcall` targets these cases with a small, predictable, compiled tool.

## Design goals

* minimalism
* statically described function set
* explicit argument descriptions
* simple CLI → libc → stdout / exit code

## Non-goals

* not a general-purpose FFI
* not a replacement for Python `ctypes`
* not a daemon or RPC service
* not designed for complex structs or pointer-heavy APIs

## Usage

```sh
lcall <function> [arg1=value1 arg2=value2 ...]
```

Arguments are passed as `name=value` pairs. Only simple types are supported:

* strings (hostnames, service names)
* integers (port numbers, indexes)

Complex types (pointers, structs) are either omitted or initialized internally.

### Examples

```sh
lcall getaddrinfo node=example.com service=80
lcall getnameinfo addr=127.0.0.1 service=80
lcall if_nametoindex ifname=eth0
lcall if_indextoname ifindex=2
```

## Functions

| Function       | Status             | Description                                  | Arguments                                    |
| -------------- | ------------------ | -------------------------------------------- | -------------------------------------------- |
| getaddrinfo    | :white_check_mark: | network address and service translation      | node=&lt;hostname&gt; [service=&lt;port&gt;] |
| getnameinfo    | :white_check_mark: | convert a socket address to host and service | addr=&lt;IP&gt; service=&lt;port&gt;         |
| if_nametoindex | :white_check_mark: | interface name → index                       | ifname=&lt;name&gt;                          |
| if_indextoname | :white_check_mark: | interface index → name                       | ifindex=&lt;number&gt;                       |

## Current status

* minimal, stable CLI utility
* all implemented functions tested
* ready for general use
* cross-platform where supported (POSIX)
