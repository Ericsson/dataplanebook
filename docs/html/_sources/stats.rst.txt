..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

Statistics
**********

Requirements
============

Uses

* Domain usage (billing etc.)
* Trouble shooting

Counters versus traces.

Requirements

* Correctness
* Transactions (correctness across multiple counters)
* Performance

Counter Organization
====================

Synchronization
---------------

Worker-worker versus worker-control plane synchronization.

Correctness Requirements and Transactions
-----------------------------------------

Separate versus Integrated Counters
-----------------------------------

Per-core Counters
-----------------

Shared Atomic Counters
----------------------

Lock Protected Shared Counters
------------------------------

Per-core Write Buffers
----------------------

Counter Reset
-------------

DPDK Device Statistics
======================

DPDK Metrics Library
====================

DPDK Telemetry Library
======================
