..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

Work Scheduling
***************

(Work scheduling is a too-narrow title. This chapter should cover the
overall organization of the processing.)

Hardware Ingress Load Balancing
===============================

Receive Side and Receive Flow Scaling
-------------------------------------

DPDK Generic Flow
-----------------

Run to Completion
=================

Pipeline
========

Optimization Points
-------------------

* Minimize packet header and buffer meta data core-to-core transition
* Minimize instruction cache footprint for a particular core
* Minimize cache working set related to per-flow, per-stage data
* Minimize cache working set related to per-stage data


Scheduled Pipeline
------------------

DPDK Eventdev
^^^^^^^^^^^^^

DPDK Service Cores
==================
