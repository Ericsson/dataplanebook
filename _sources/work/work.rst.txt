..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

.. _Work Scheduling:

Work Scheduling
***************

Introduction
============

This chapter discusses how to organize processing in the data plane,
to achieve the appropriate level of :term:`parallelism` and, as the
end goal, meet the system's characteristics requirements.

The issue boils down to how to distribute work across multiple CPU
cores, while maintaining correctness, cycle and energy efficiency and
scalability.

As discussed in the :ref:`chapter on threading <Threading>`, a data
plane application is generally required to set up a system of
:term:`threads <Operating system thread>`-pinned-to-:term:`cores <CPU
core>` — a scheme which effectively disables operating system-level
:term:`multitasking`.

After throwing the standard vehicle for load balancing out the window,
the data plane application needs a replacement. That replacement is
the topic of this chapter.

The related but distinct question of how to decouple different
protocol layers (and other modules) in the data plane application will
be covered in a future chapter of :ref:`Modularization`.

Although the DPDK :term:`service cores framework` can be seen as a
static load balancer, the primary function of that function is
decoupling (and :term:`concurrency`), and thus will be also be covered
in that chapter.

Basic Concepts
==============

Item of Work
------------

This book will use the term :term:`item of work` to denote the
smallest unit of work handed to a :term:`EAL thread`.

A typical item of work consists of information on how the work
scheduler treat this item, and a pointer to a packet (or a list of
packets), and some additional meta data, like the application-level
destination (identifying the processing stage).

At a bare minimum, the an item of work handed from the work scheduler
to the receiving party in the application must carry enough information
so the that receiver knows what to do with it.

Similarly, when an item of work is being added to the work scheduler,
it must carry enough information so the that work scheduler knows what
to do with it.

An item of work may also represent a timeout event, a completion
notification from an accelerator, or a request from :term:`data plane
control` to update a table, or retrieve some information about the
state of the data plane.

In DPDK `<rte_eventdev.h>
<https://doc.dpdk.org/api/rte__eventdev_8h.html>`_ an item of work is
referred to as an *event*.

For non-scheduled data planes, the item of work simply consist of a
pointer to the packet buffer (and its meta data).

In the context of an operating system process scheduler, the item of
work is a runnable process, thread, or (in the case of Linux)
task. For the process scheduler, an item of work always contains a
stack, register state (including a program counter). An item of work
need not, and for the purpose of this chapter, does not.

The way the application communicate details about how an item of work
should be treated is also different between a process scheduler, and
a data plane work scheduler.


Work Scheduler
--------------

For the purpose of this book, a work scheduler is a data plane fast
path function that distributes :term:`items of work <Item of work>`
across the :term:`fast path lcores <Fast path lcore>`.

The work scheduler takes as input a set of items of work, and decides
how to schedule the tasks across available lcores, considering the
constraints (ordering, atomicity guarantees).

From a characteristics point of view, the overall goals of the work
scheduler are:

* Maximize throughput (best-case, worst-case, and/or something in-between).
* Minimize :term:`latency <Wall-clock latency>` (average and/or at the
  tail end).
* Maximize resource efficiency (e.g., power, CPU core count, or DRAM).
* Maximize fairness (or more general, maintain to appropriate quality
  of service, which may not be fair at all).

Depending on the application, different weights will be placed on the
different work scheduler sub goals.

More in detail what kind of functionality (e.g., treatment) the work
scheduler will provide, and how work is fed into and retrieved from
the machinery varies depending on the work scheduler implementation.

While this description may bring something like a work scheduler of
for the scheduled pipeline to mind, a simple function such as
:term:`RSS` function of a term:`NIC` can also be made to serve as a
work scheduler.

Work Scheduling Models
======================

Run to Completion
-----------------

An application using :ref:`data plane threading <Data Plane
Threading>`, where each :term:`EAL thread` is assigned one or more
items of work, and continues process of those tasks, without any
interruption, until they are finished. *Finished* here means that all
application-internal state changes related to that input has occurred,
and any and all output related to those set of inputs that can been
produced, have been produced.

Outputs which cannot be produced because some information is not yet
available, or where the output must be produced at some particular
time, is exempted.

Thus, in a system implementing strict run-to-completion by this
definition, a thread to not hand off work to another thread, provided
the work could be performed immediately.

The archetypal example of a run-to-completion DPDK data plane
application is with a number of :term:`EAL threads <EAL thread>`, one
per CPU core. Each thread is assigned one RX and TX NIC queue. Upon
receiving a packet, a thread picks up the packet, performs all the
processing required (e.g., runs all the network layers), and ends
producing a packet being sent out on one of the thread's NIC TX
queues, without any interruptions.

A :ref:`standard threading <Standard Threading>` model data plane
application using :term:`preemptable threads <Preemptable thread>` and
blocking system function calls to access the network stack may seem to
run-to-completion from strictly source code point of view. However,
since the threads may be interrupted, this is not usually what's meant
with run-to-completion. The same is true for software using coroutines
or other green threading techniques.

Parallels to Other Domains
^^^^^^^^^^^^^^^^^^^^^^^^^^

In general, run-to-completion is used to describe a system, where a
thread is assigned a task and continues execution until the task is
finished, without any interruptions.

In the context of operating system process scheduling,
run-to-completion is a synonym for threads being
:term:`non-preeemptable <Non-preemptable thread>`, and the use of
cooperative multitasking (i.e., the running thread runs until it
voluntarily gives up the CPU). As outline in the :ref:`Threading`
chapter, data plane threads are never to be interrupted for any length
of time, regardless of run-to-completion of some other work scheduling
model is used, so this usage does not make sense if :ref:`data plane
threading <Data Plane Threading>` is used.

Run-to-completion in the context of finite state machine machines
means that a state machine finishes the processing of a particular
event, before it initiates processing of the next. Such state machines
are not parallel, while the data plane processing almost always is.
DPDK Eventdev maps more closely to an actor model, with the difference
that eventdev events are not the *only* means of communication between
actors (shared memory is also allowed).

In a single-threaded UNIX application also employs run-to-completion,
in the sense no blocking system calls are made, and the processing of
an event either finishes, or the thread stores whatever state is
required for further, future, processing, and explicitly yields the
thread to allow it to be reused to process some other event. It's not
run-to-completion in the sense that the threads are not preempted.

This interruption-free operation is relevant from a performance point
of view, and living without multitasking has software
architecture-level impact.

Hardware Ingress Load Balancing
===============================

Receive Side and Receive Flow Scaling
-------------------------------------

DPDK Generic Flow
-----------------

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

Scheduling Type
---------------

DPDK Eventdev
^^^^^^^^^^^^^

DPDK Service Cores
==================
