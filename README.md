[![Build Status](https://travis-ci.org/martinusbach/vt-timers.svg?branch=master)](https://travis-ci.org/martinusbach/vt-timers)
[![Build status](https://ci.appveyor.com/api/projects/status/56wbff12087v5el4/branch/master?svg=true)](https://ci.appveyor.com/project/martinusbach/vt-timers/branch/master)

Profiling timers
================

General description
-------------------

Library to instrument a code with timers and report the results in a tree view. Works with multiple threads (incl. OpenMP).


Building
--------

- Build with `CMake`.
- Contains tests based on `google test`, which is downloaded automatically during CMake generation time. Test targets and google test framework are only built if `PROFILING_TIMERS_ENABLE_TESTS` is switched `ON`.
- Requires a C++11 compiler. Tested with Visual Studio 2015 and GCC under linux. Compiles with MinGW, but crashes, see below.


Known issues
------------

- There is a bug in C++11 `thread_local` storage with the MinGW compiler, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83562 and https://github.com/ChaiScript/ChaiScript/issues/402. Confirmed with 32 and 64 bit variants of MinGW 8.1.0.


Suggestions for future work
---------------------------

- compute 'other' time (non-labeled remaining time within a Timer scope)
- include cpu time (user, other)
- aggregate timers from different threads
- add possibility to send report to ostream
- add a scoped timer, to ensure exception safety
- reconsider the use of `std::map` (e.g. is `std::unordered_map` better? should we build our own data structure based on indices instead of key-value pairs based on strings?)
- does it work with other compilers?
- does it work with pthreads?
- add option to disable all timer function calls (e.g. via a preprocessor define that may or may not include `#if 0`)
- ensure correct workings with shared libraries (in view of `static` and `static thread_local` usage)
- define proper C-interface
- add fortran interface
- use python to visualize a timing report
- suggestion: if we encouter problems, it may be useful to consider omp threadlocal variables.


History
-------

This code was inspired and partially based on the Fortran code `TimeKeeper` developed at VORtech (Delft).


License
-------

This software is licensed under the MIT license, see LICENSE.MIT.
