# MINCE

## Overview

* **MINCE** (MINCE Is Not Completely EMACS) is an Emacs-like text editor from **Mark of the Unicorn**

* It was available for *CP/M-80*, *CP/M-68K*, *PDP/RSX-11*, and *UNIX* operating systems

## Portable MINCE

* "**Portable MINCE**" is the last **CP/M-80** release of **MINCE**, plus:
  * a transparent **CP/M 2.2** translation layer
  * an **8080** CPU emulator (_based on **COM**_)

## Building

* Building from source requires:
  * C89 C compiler (*`gcc` and `clang` tested*)
  * GNU `make`
  * POSIX `sed`
  * Tcl `expect`
  * `coffwrap` *or* POSIX `od`

* Tested on:
  * GNU/Linux x86_64, Fedora 33 (2020-11-29), GNU C 10.2.1 (Red Hat 10.2.1-9)
  * GNU/Linux armv7h, Raspbian Buster 10 (2021-03-04), GNU C 8.3 (Raspbian 8.3.0-6+rpi1)
  * GNU/Linux arm64, Debian Bullseye 11 (2021-04-03), GNU C 10.2.1 (Debian 10.2.1-6)
  * Darwin x86_64, macOS Big Sur 11.3 DP 6 (2021-03-29), Xcode 12.4, Apple clang 12 (clang-1200.0.32.29)

## TODO

* Configuration system, customizable by a file or environment variables.
* Translation of line-endings at runtime (*for now `dos2unix` can be used*).
* Translation for `PgUp`/`PgDn`/`Home`/`End`/cursor control (*arrow*) keys.
* Support for automatic terminal resizing (_by catching and handling **SIGWINCH**_).
* Build and support MOTU Scribble and Crayon in similar fashion.
* Include 68000 CPU core and include MINCE-68K & SCRIBBLE-68K.
  * Automatically invoke 68K versions for large input files.
  * Allow the user to default to MINCE-80 or MINCE-68K via configuration.
* User-configurable key bindings without requiring recompilation.
