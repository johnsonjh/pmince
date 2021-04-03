# MINCE

* ***MINCE Is Not Completely EMACS***

## Overview

* **MINCE** is an Emacs-like text editor from **Mark of the Unicorn, Inc.**
* Originally available for **CP/M-80**, **CP/M-68K**, **PDP/RSX-11**, and **UNIX** systems

## Portable MINCE

* "**Portable MINCE**" is based on the last release of **MINCE** (*2.6*), plus:
  * **CP/M** system call translation
  * **8080/Z80** CPU emulation

## Building

* Build requirements:
  * C compiler (`gcc`, `clang`, etc.)
  * GNU `make`
  * Tcl `expect`
  * POSIX `env`, `sh`, `sed`, and `od` (*or* `coffwrap`)

## Tested configurations

* GNU/**Linux**
  * x86\_64 (AMD64)
    * **RHEL** 8.3, **GNU C 8.4.1** (*Red Hat 8.4.1-1*)
    * **CentOS** 8 (Stream), **GNU C 10.2.1** (*Red Hat SCL 10.2.1-7*)
    * **Fedora** 33, **GNU C 10.2.1** (*Red Hat 10.2.1-9*)
    * **Fedora** 33, **Intel oneAPI DPC++ 2021.2.0** (*2021.2.0.20210317*)
  * ARMv7HF (ARM32-GNUEABI)
    * **Raspbian** (Buster) 10, **GNU C 8.3.0** (*Raspbian 8.3.0-6+rpi1*)
  * ARM64 (AArch64)
    * **Debian** (Bullseye) 11, **GNU C 10.2.1** (*Debian 10.2.1-6*)
* Apple/**Darwin**
  * x86\_64 (AMD64)
    * **macOS** (Big Sur) 11.3 Beta 6, **Xcode 12.4** (*clang-1200.0.32.29*)

## Roadmap / TODO

* End-user configuration system, customizable via file or environment variables.
* Avoid main input busy-wait/spin loop to reduce idle CPU usage.
* Translation of line-endings at run time (*for* *now* `dos2unix` *may* *be* (*ab*)*used*).
* Translation for `PgUp`/`PgDn`/`Home`/`End`/cursor control/arrow keys.
* Support for automatic terminal resizing (_by catching and handling **SIGWINCH**_).
* Build and support MOTU **Scribble**, **Pencil**, and **Crayon** in similar fashion.
  * Automatic setup for **ANSI**/**ASME** _**Y14.1**_ (*US*) and *ISO*-_**216**_ (*EU*) paper types.
  * End-user run time switching between different printer defintions.
  * Support for rendering of proportional (*NEC*, *Epson*, *Diablo*, etc.) printer output.
  * **Ghostscript**/**ImageMagick** for printer output conversion to *PDF*/*PostScript*/*PCL*/*XPS*/*PNG*/*etc*.
* Add Motorola **68000** CPU core, to support **MINCE-68K**.
  * Automatically invoke **MINCE-68K** version for large input files.
  * End-user configuration of default **MINCE** flavor (**MINCE-80** or **MINCE-68K**).
    * Native execution (*translation*) for **MINCE-68K** for **M68000** systems.
* User-configurable key bindings and remapping.
  * Allow user configuration *without* requiring full recompilation.
  * Normalize key bindings and mappings between **MINCE** flavors.
* Support for *DEC* **RSX-11**, *DEC*/*Compaq*/*HPe*/*VMSI* _(*Open*)_**VMS**, and *Microsoft* **Windows** operating systems.

## Original Authors

* **Portable MINCE** by *[Jeffrey H. Johnson](https://github.com/johnsonjh/pmince)* *\<[trnsz@pobox.com](mailto:trnsz@pobox.com)\>*
* **MINCE** *2.6*: Copyright (C) 1980-1982 by **Mark of the Unicorn, Inc.**
* **DIFF** *1.1*: Copyright (C) 1981 by **Mark of the Unicorn, Inc.**
* **Scribble**, **Pencil**, **Crayon** *1.3*: Copyright (C) 1981 by **Mark of the Unicorn, Inc.**
  * **MINCE**/**DIFF**/**Scribble**/**Pencil**/**Crayon**: Authored by *Jason* *T.* *Linhart*, *Scott* ("*Gyro*") *Layson* *Burson*, *Craig* *A.* *Finseth*, *Brian* *N.* *Hess*, and *Bill* *Spitzak*.
* **CCOM80** (_Derived from **COM** 3.0_): Copyright (C) 1984-2006 by *Jim* *Cathey*, *Edmund* *Ramm*, *Charlie* *Gibbs*, and *Willi* *Kusche*.
