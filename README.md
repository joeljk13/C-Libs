The Modules
===========

Whenever I think of a module that would help in general with C, this is where
I'll develop it.

The Framework
=============

I created this for 2 reasons:

* I wanted to run testing with make check, and existing frameworks didn't seem
    to do that.
* I wanted GCC/Clang to generate the dependencies, and I didn't know of a way to
    do this without a wrapper shell script (configure) around a makefile.

Basically, running configure will create the makefile, with updated
dependencies and with support for make, make clean, and make check. A sources
in src/ are used, and all tests in tests/ are used for make check.
