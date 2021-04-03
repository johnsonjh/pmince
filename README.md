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
  * Linux/x86_64, GNU C
  * Linux/armv7h, GNU C
  * Linux/arm64, GNU C
  * Darwin x86_64, Apple clang

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
