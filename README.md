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
  * POSIX `cmp`, `env`, `sed`, and `sh`

## Tested configurations

* i686 (IA32)
  * **Linux**, GNU C 10.2.0
* x86\_64 (AMD64)
  * **RHEL** 8.3, GNU C 8.4.1
  * **CentOS** 8 (Stream), GNU C 10.2.1 (*SCL*)
  * **Fedora** 33, GNU C 10.2.1
  * **Fedora** 33, Intel oneAPI DPC++ 2021.2.0 (*20210317*)
  * **macOS** (Big Sur) 11.3 Beta 6, Xcode 12.4 (*clang-1200.0.32.29*)
  * **OpenBSD** 6.8, OpenBSD clang 10.0.1
* ARMv7HF (ARM32)
  * **Raspbian** (Buster) 10, GNU C 8.3.0 (*6+rpi1*)
* ARM64 (AArch64)
  * **Debian** (Bullseye) 11, GNU C 10.2.1
  * **FreeBSD** 13.0-RC4, FreeBSD clang 11.0.1 (*g43ff75f2c3fe*)
* RISC-V (RV64)
  * **Linux**, GNU C 10.2.0

## Roadmap

* Eliminate `expect` from the build requirements.
* End-user configuration system, customizable via file or environment variables.
* Avoid main input busywait/spin-loop to reduce idle CPU usage.
* Translation of line-endings at run time (*use* `dos2unix` *as* *a* *workaround*).
* Translation of `PgUp`/`PgDn`/`Home`/`End`/cursor control/arrow keys.
* Build with GCC `-fno-common` and reduce `-Wall` warnings.
* Support crash recovery (by parsing `mince.swp`) in case of unclean exit.
* Add support for external filters, such as hexadecimal mode (via `xxd`).
* Support for automatic terminal resizing (_by catching and handling **SIGWINCH**_).
* Allow building **MINCE** binaries with various extensions; produce verified/tested configurations.
* Build and support MOTU **Scribble**, **Pencil**, and **Crayon** in similar fashion.
  * Automatic setup for **ANSI**/**ASME** _**Y14.1**_ (*US*) and *ISO*-_**216**_ (*EU*) paper types.
  * End-user run time switching between different printer defintions.
  * Support for rendering (*NEC*, *Epson*, *Diablo*, etc.) proportional font printer output.
  * **Ghostscript**/**ImageMagick** for printer output conversion to *PDF*, *PostScript*, etc.
* Add Motorola **68000** CPU core, to support **MINCE-68K**.
  * Automatically invoke **MINCE-68K** version for large input files.
  * End-user configuration of default **MINCE** flavor (**MINCE-80** or **MINCE-68K**).
    * Native execution (*translation*) for **MINCE-68K** for **M68000** systems.
* User-configurable key bindings and remapping.
  * Allow user configuration *without* requiring full recompilation.
  * Normalize key bindings and mappings between **MINCE** flavors.
* Support for **RSX-11**, (**Open**)**VMS**, **Windows**, and **OS/2** operating systems.

## Original Authors

* **Portable MINCE** contact: *[Jeffrey H. Johnson](https://github.com/johnsonjh/pmince)* \<[trnsz@pobox.com](mailto:trnsz@pobox.com)\>.
* **MINCE** *2.6*: Copyright © 1980-1982 by **Mark of the Unicorn, Inc.**
* **DIFF** *1.1*: Copyright © 1981 by **Mark of the Unicorn, Inc.**
* **Scribble**, **Pencil**, **Crayon** *1.3*: Copyright © 1981 by **Mark of the Unicorn, Inc.**
  * **MINCE** / **DIFF** / **Scribble** / **Pencil** / **Crayon**: Authored by *Jason* *T.* *Linhart*, *Scott* ("*Gyro*") *Layson* *Burson*, *Craig* *A.* *Finseth*, *Brian* *N.* *Hess*, and *Bill* *Spitzak*.
* **CCOM80** (_Derived from **COM** 3.0_): Copyright © 1984-2008 by *Jim* *Cathey*, *Edmund* *Ramm*, *Charlie* *Gibbs*, and *Willi* *Kusche*.
* **coffwrap** (_Derived from **bin2c**_): Copyright © 2015 by *Ondrej* *Novak*.

## License

* See the [LICENSE](/LICENSE.md) file for important details.
