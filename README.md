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
  * C89 C compiler
  * GNU Make
  * POSIX sed
  * Expect

## TODO

* Transparent translation of *UNIX* and *DOS* line-endings at runtime (*for now `dos2unix` can be used*).
* Transparent translation of `PgUp`, `PgDn`, `Home`, `End`, `Insert`, and cursor control (*arrow*) keys.
* Support for automatic terminal resizing (_by catching and handling **SIGWINCH**_).
