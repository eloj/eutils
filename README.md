# C11+ Utility Functions & Macros

[![Build status](https://github.com/eloj/eutils/workflows/build/badge.svg)](https://github.com/eloj/eutils/actions/workflows/build.yml)

A collection of useful standalone C11+ functions and macros.

I don't make _any_ guarentees, and I may change interfaces and implementations at any point.

I don't expect anyone else to use any of this, but you're welcome to it if you want.

## Building

There's currently nothing to build, it's just a header.

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
$ pkg-config eutils --cflags
-I/home/user/local/include
```

## License

All code is provided under the [MIT License](LICENSE).
