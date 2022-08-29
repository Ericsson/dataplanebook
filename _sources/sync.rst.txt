..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

Synchronization
***************

Staying Safe
============

Thread Safety
-------------

Asynchronous Signal Safety
--------------------------

Preemption Safety
-----------------

Considerations
==============

* Read access frequency, and cardinality.
* Write access frequency, and cardinality.
* Needs changes be applied applied atomically across a set of data items?
* Resource management

Messaging Versus Shared Memory

Amdahl's Law
============

Critical Section Length and Impact
----------------------------------

C11 Atomics
===========

Sequence Locks
==============

Spinlocks and Ticketlocks
=========================

Read-Copy-Update
================

Per Core Data Structures
========================

(Really per-thread data structures in DPDK.)
