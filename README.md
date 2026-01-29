# lcall

`lcall` is a minimalistic CLI utility for calling selected `libc` functions
(and potentially functions from other system libraries) directly from the
command line.

The project is intended as a lightweight bridge between shell scripts and
POSIX APIs, without using high-level languages, interpreters, or heavy
runtime dependencies.

## Motivation

Sometimes you just want to:

- call a system function directly from a shell script
- use OS-provided mechanisms (DNS resolver, time, filesystem, etc.)
- avoid Python/Perl for a single libc call
- have a small, compiled, predictable tool

`lcall` targets these cases without trying to be a universal FFI.

## Design goals

- minimalism
- statically described function set
- explicit argument descriptions
- no virtual machines or persistent state
- CLI → libc → stdout / exit code

## Non-goals

- not a general-purpose FFI
- not a replacement for Python ctypes
- not RPC, not a daemon (at least for now)
- not suitable for arbitrary structs or pointer-heavy APIs

## Usage (concept)

```sh
lcall getaddrinfo node=example.com service=80
```

Arguments are passed as `name=value` pairs.

Only simple argument types are supported:

- strings
- integers

Complex arguments (`struct`, pointers, callbacks) are either:

- omitted
- passed as `NULL`
- initialized with reasonable defaults inside the wrapper

## Current status

- early prototype
- unstable API
- very small function set
- developed incrementally based on real use cases

| f()               | :x: :white_check_mark: | Description                                                  |
| ----------------- | -----------------------| -------------------------------------------------------------|
| getaddrinfo       | :white_check_mark:     | network address and service translation                      |
| getnameinfo       | :white_check_mark:     | convert a socket address to a corresponding host and service |
| if_nametoindex    | :white_check_mark:     | interface names to indexes conversion                        |
| if_indextoname    | :white_check_mark:     | interface indexes to names mapping                           |
| setpriority       | :white_large_square:   | set program scheduling priority                              |
| getpriority       | :white_check_mark:     | get program scheduling priority                              |

## Possible future directions

- support for additional libraries via `dlopen`
- optional daemon mode with persistent state
- extended argument types
- IDL-based function descriptions

Or it may get rewritten entirely. That is also a valid outcome.
