# MINCE

## Overview

* **MINCE** (MINCE Is Not Completely EMACS) is an Emacs-like text editor from **Mark of the Unicorn**

* It was available for *CP/M-80*, *CP/M-68K*, *PDP/RSX-11*, and *UNIX* operating systems

## Portable MINCE

* "**Portable MINCE**" is based on the last release of **MINCE** (2.6), plus:
  * **CP/M 2.2** system call translation
  * **8080/Z80** CPU emulation

## Building

* Building from source requires:
  * C89 C compiler (*`gcc` and `clang` tested*)
  * GNU `make`
  * POSIX `sed`
  * Tcl `expect`
  * `coffwrap` *or* POSIX `od`

* Tested on:
  * GNU/**Linux**, x86\_64
    * **Fedora** 33, **GNU C 10.2.1** (*Red Hat 10.2.1-9*)
    * **Fedora** 33, **Intel(R) oneAPI DPC++ Compiler 2021.2.0** (*2021.2.0.20210317*)
  * GNU/**Linux**, armv7hf
    * **Raspbian** (Buster) 10, **GNU C 8.3.0** (*Raspbian 8.3.0-6+rpi1*)
  * GNU/**Linux**, arm64
    * **Debian** (Bullseye) 11, **GNU C 10.2.1** (*Debian 10.2.1-6*)
  * Apple/**Darwin**, x86\_64
    * **macOS** (Big Sur) 11.3 Beta 6, **Xcode 12.4** (*clang-1200.0.32.29*)

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
