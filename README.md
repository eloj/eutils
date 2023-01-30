# C11+ Utility Functions & Macros

[![Build status](https://github.com/eloj/eutils/workflows/build/badge.svg)](https://github.com/eloj/eutils/actions/workflows/build.yml)

A collection of standalone C11+ functions and macros.

I don't expect anyone else to use any of this, but you're welcome to it if you want.

If you learn or use something from this repo, give it a star. Cost nothing.

## Caveat Emptor

If you are one of those programmers who insist your code must build on a
toolchain from 1989, and hate it when libraries improve their API, then
you are in the wrong place.

I don't provide _any_ guarentees whatsoever about this code. I may rename files,
change interfaces, rebase master and so on, at any point in time.

## Building

There's currently nothing to build, it's just headers.

Run tests with:

```bash
$ make test
```

## Installation

You can copy and paste, curl or wget, or clone the repo to your heart's content.

As an option, header(s) plus a `pkg-config` configuration can be installed:

```bash
$ PREFIX=$HOME/local make -B install
...
$ pkg-config --cflags eutils
-I/home/user/local/include/eutils
```

## License

All code is provided under the [MIT License](LICENSE).
