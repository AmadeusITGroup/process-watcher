Copyright (C) 2025 Amadeus s.a.s.
See the end of the file for license conditions.

process-watcher
===============

Measure the peak (envelope) memory consumption of trees of processes
over a time window identified by PID.

The process-watcher is a tool that helps measure the memory usage of
any process (and its subprocesses, recursively).  For example, it
can measure how much memory is needed to run "make".

In Amadeus, it is used by an internal build tool to report the memory
envelope used by each part of the build of a project, so that
developers can know and request the right-sized amount of memory for
their needs, and tune the parallel options of their build to reduce
the build duration as much as possible without exceeding the available
memory (and avoid being killed by the Linux OOM killer).

As it identifies a process tree by the PID of the top process, it is
suitable to identify a particular process tree and not something like
"all processes that are named gcc on the system", which would be
problematic if several similar jobs may be running at the same time.

process-watcher is designed to have a very small disk footprint,
memory footprint and cpu footprint, which makes its installation and
use unobtrusive even in very constrained or saturated environments.

How process-watcher works
=========================

The "process-watcher capture" continuously monitors all processes of
the system (specifically by watching /proc), and records their parent
PID, and their memory counters, every 2 seconds (the sampling rate).
It keeps the whole history of this information; on my desktop
computer, each snapshot adds about 30 KB to the history file, but you
can reduce it by removing unwanted stats from the "fields" file and
recompiling.  It consumes very little memory and CPU (though it should
be possible to optimize it even more, see TODO.md).  It needs to be
stopped (e.g. kill -TERM) when the monitoring is not needed anymore.

It also provides a query endpoint "process-watcher get".  This
endpoint allows querying the maximum memory usage of a particular
process tree over a particular time window (start and end time).  It
can be run while the capture is running.  On my laptop, it is able
to process a 700+ MB history file in under 880 ms, so about 800 MB/s.

Limitations
===========

Note that if a process starts and terminates within a sampling period,
then it will go undetected by process-watcher.  However, the main use
of process-watcher is to find processes that consume a lot of memory,
and those processes generally last much longer than the sampling
period.

Similar works
=============

https://github.com/ncabatoff/process-exporter (a Prometheus exporter)
is a Go program that regularly checks the statistics of processes and
exports them to a Prometheus server.  It cannot measure a specific
process tree if there are multiple process trees matching your
matchers, or measuring the memory envelope of a process tree, but is
useful for getting statistics over a long time.

https://github.com/astrofrog/psrecord is a Python program that can
collect memory and CPU stats of a process tree identified by its top
PID.  It does not compute the envelope memory statistics, and has a
larger overall footprint as it relies on CPython.  However, it has
features that process-tree does not have, such as the CPU stats.

Building
========

Prerequisites:

- autoconf, automake
- some C compiler (gcc...)
- some awk implementation.

To build:

    ./autogen.sh
    ./configure
    make check

To install:

    make install

License
=======

This file is part of process-watcher.

process-watcher is free software: you can redistribute it and/or
modify it under the terms of the Apache 2.0 License as published by
the Apache Software Foundation.

process-watcher is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the License
for more details.

You should have received a copy of the Apache 2.0 License
along with process-watcher.
If not, see <https://www.apache.org/licenses/LICENSE-2.0>.
