..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

.. _Work Scheduling:

Work Scheduling
***************

Introduction
============

This chapter discusses how to organize processing in the data plane,
in particular how achieve the appropriate level of :term:`parallelism`
and, in the end, meet the system's characteristics requirements.

The core issue is to how to distribute work across multiple CPU cores,
while maintaining correctness, cycle and energy efficiency and
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
smallest job assigned to a :term:`EAL thread`.

A typical item of work consists of information on how the work
scheduler should (or must) treat this item, and a pointer to a packet
(or a list of packets), and some additional meta data, like the
application-level destination (identifying the processing stage).

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
referred to as an *event*. The same term is used by the `Open Event
Machine <https://openeventmachine.github.io/em-odp/>`_, a
eventdev-like mechanism for :ref:`ODP`.

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

The *work scheduler* of this book is a data plane fast path function
that distributes :term:`items of work <Item of work>` across
:term:`fast path lcores <Fast path lcore>`.

The work scheduler takes as input a set of items of work, and decides
how best to distribute the work across available lcores, maintaing the
constraints (e.g., ordering requirements) given by the application.

From a characteristics point of view, the overall goal of the work
scheduler can be summarized as:

* Maximize throughput (best-case, worst-case, and/or something in-between).
* Minimize :term:`latency <Wall-clock latency>` (average and/or at the
  tail end).
* Maximize resource efficiency (e.g., power, CPU core count, or DRAM).
* Maintain fairness (or more general, maintain appropriate quality
  of service, which may not be fair at all).

Depending on the application, different weights will be placed on the
different work scheduler sub goals.

Conceptually, data plane work scheduler is very similar to a generic
operating system kernel process scheduler. See the :ref:`Process
Scheduler` section.

What kind of functionality (e.g., treatment) the work scheduler will
provide, and how work is fed into and retrieved from the machinery
varies depending on the work scheduler implementation.

The work scheduler may be implemented in software (in the
:term:`platform <Data Plane Platform>`, or the application), or in
hardware, or a combination of the two.

A data plane work scheduler may take many different forms, ranging
from something that adhere to from `Eventdev
<https://doc.dpdk.org/guides/prog_guide/eventdev.html>`_ to a
relatively simple function such as :term:`RSS` of :term:`NIC`
(combined with the appropriate configuration and RX queue setup).

Work Size
---------

The per-packet :term:`processing latency` in the data plane is short
compared to the cost of serving a request in most other types of
network applications. See the section on :ref:`Data Plane
Applications` for more on this topic.

The items of work managed by the data plane work scheduler are
dominated by items directly related to the (partial, or complete)
processing of a particular packet. To improve work scheduling
efficiency (among other things), an item of work may reference
multiple packets. Furthermore, when the work scheduler hands over work
to the application, it may do so in batches, to reduce scheduling
overhead (and give ample opportunity for :term:`software
prefetching`).

However, there also forces that work in the opposite direction, toward
a preference of short (batches of) jobs:

* There are no means for the work scheduler to interrupt an on-going
  execution. This in turn will cause head-of-line blocking, where a
  low-priority job may prevent the work scheduler from immediately
  scheduling an arriving high-priority job for execution.
* A system could wait to gather a large batch of packets, to reap the
  efficiency benefits of batching. [#vpp]_ However, such buffering
  will add to the port-to-port :term:`wall-clock latency`.

One often-viable approach is to have a system where limited or no
batching of packet (or items of work) occurs at low load, and
increases with increased system capacity utilization. If the system is
working at the limits of its capacity, a large amount of batching is
usually the best option. At this operating point, the typically
somewhat bursty nature of arriving packets will leave the system, at
least at times, with a backlog, forcing the use of buffering (or
dropping packets). Batching in this scenario allows the system to
exploit the efficiency gains, potentially avoiding further buffering
(and the associated increase in packet delays) to occur.

For systems which can scale hardware resources (e.g., CPU cores) very
quickly, it may be beneficial to keep the system running on
fewer-than-available cores, and keep those cores relatively busy,
leading to batching effects, and in turn higher power efficiency (at
the cost of increased port-to-port wall-time latency experienced by
forwarded packets).

Flow
----

The data plane work scheduler must often track relationships between
an :term:`item of work` and other items (see the section on :ref:`Work
Ordering` and :ref:`Scheduling Types`).

For tracking arrival order, or processing order, the concept of a
*flow* is often useful. The flow of this chapter is often, but does
not have to be, related to some flow-like concept in a network
protocol layer.

A common *flow* for applications operates in the Internet
:term:`network protocol suite` is that flows are made up of IP packets
pertaining to the same TCP connection (or UDP flow), or all packets
going in a particular direction, between two IP hosts.

It's often useful to map several network-level flows to a single item
of work flow. This may be done both to reduce the number of flows the
work scheduler must handle, or extend the ordering guarantees given by
the different scheduling types to include more than one flow. One
example is to classify both uplink and downlink TCP frames into the
same atomic flow, to reduce synchronization overhead and cache
locality for data pertaining to both directions, for that TCP
connection.

It's also often useful to map a single network-level flow, in one
particular direction, to multiple item of work flows, in an
application implementing the pipeline pattern. Each such flow
represent a processing stage. See the :ref:`Pipeline` section for
more information.

The term *ordering context* is sometimes (e.g., in :term:`NPUs <NPU>`)
used for an item of work flow.

.. _Work Ordering:

Work Ordering
-------------

A :term:`network function` performing packet forwarding (in a general
sense) is almost always required to maintain some kind of order of
packets coming in, and the corresponding packets going out. In other
words, when packets arrive in some well-defined order at the
:term:`network function` ingress, any packets produced as a results of
processing those packets, should egress the system in the same order.

:term:`Network function`-level packet ordering requirements directly
translate into a set of requirements on the data plane work scheduler
(and associated machinery, like a packet-to-flow classification
function).

Generally, causing reordering is not strictly prohibited, and its
better to deliver out-of-order packets than to drop them. For example,
in an IP network, a stream of packets originating from some particular
host may be reordered as they traverse the network. Reordering of
packets may result in serious performance degradations.

For example, :term:`Transport layer <Layer 4>` protocols may infer
packet loss from the arrival of out-of-order packets, and signaling to
the remote peer retransmit the data.

At a minimum, reordering incurs an addition processing cost on the end
systems.

Typically, the data plane application need not maintain a total order
between input and output, but it's enough to maintain the packets
pertaining to the same network flow.

Maintaining a total order of all packets may not even be preferably,
since it would prevent the system prioritize some packets, over
others. It could also cause head-of-line blocking, where a single
time-consuming-to-process packet prevents other packets from being
forwarded.

A requirement to maintain the packet order from :term:`network
function` input to output does not itself force the processing of
packets to happen in order.

In some cases, the actual work should (or must) be performed in the
same order the packets arrived. One example is PDCP sequence number
allocation, which generally should be done in the same order as the

For a software work scheduler, it may be to more cycle-efficient to
maintain order by also imposing in-order processing (i.e., one flow
goes to one core, rather than one flow goes to many flows, and the
resulting packets are reordered to restore the original arrival
order).

Work ordering is one of the aspects that differentiates a data plane
work scheduler from a operating system process scheduler.

.. _Scheduling Types:

Scheduling Types
^^^^^^^^^^^^^^^^

This chapter will reuse the terminology used by `Eventdev
<https://doc.dpdk.org/guides/prog_guide/eventdev.html>`_.

.. _Atomic Scheduling:

Atomic Scheduling
"""""""""""""""""



Load Balancing
--------------

Static versus Dynamic.

Work Scheduling Models
======================

.. _Run to Completion:

Run to Completion
-----------------

This section describes a simple work scheduling model, which in its
most basic form, work isn't really scheduled at all.

The run-to-completion model entails:

1. Use :ref:`data plane threading <Data Plane Threading>`.
2. Have every :term:`EAL thread` poll a set of sources of work.
3. Upon retrieving an item of work (e.g., by retrieving a packet from
   a :term:`NIC` RX queue) the EAL thread will continue to
   work on this task, without interruption, until it is finished.

*Finished* here means that all application-internal state changes
related a particular input has occurred, and any output related to
those set of inputs that can been produced, have been produced.

Outputs which cannot be produced because some information is not yet
available, or where the output must be produced at some particular
time, is exempted.

Thus, in a system implementing strict run-to-completion by this
definition, a thread can not hand off work to another thread, provided
the work could be performed by the original thread.

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
since the threads may be interrupted, such a design fails to qualify
as run-to-completion according to this definition. The same is true
for software using coroutines or other green threading techniques.

The driving force for this nearly interruption-free operation is
performance, living without multitasking has software
architecture-level impact.

Parallels to Other Domains
^^^^^^^^^^^^^^^^^^^^^^^^^^

In general, run-to-completion is used to describe a system, where a
thread is assigned a task and continues execution until the task is
finished, without any interruptions.

In the context of operating system process scheduling,
run-to-completion is a synonym cooperative multitasking, where a
thread is never :term:`preempted <Non-preemptable thread>` and runs
until it voluntarily gives up the CPU. As outline in the
:ref:`Threading` chapter, data plane threads are never supposed to be
interrupted for any length of time, regardless if run-to-completion or
some other work scheduling model is used, so this usage does not make
sense if :ref:`data plane threading <Data Plane Threading>` is used.

Run-to-completion in the context of finite state machine machines
means that a state machine finishes the processing of a particular
event, before it initiates processing of the next. Such state machines
are not parallel, while the data plane processing almost always is.

DPDK Eventdev maps more closely to an actor model, with the difference
that eventdev events are not the *only* means of communication between
actors (e.g., using shared memory is also allowed).

In a single-threaded UNIX application also employs run-to-completion,
in the sense no blocking system calls are made, and the processing of
an event either finishes, or the thread stores whatever state is
required for further, future, processing, and explicitly yields the
thread to allow it to be reused to process some other event. It's not
run-to-completion in the sense that the threads are not preempted.

Properties
^^^^^^^^^^

Pros

* Straight-forward implementation
* No expensive hand-offs between cores
* Trivial maintain input packet order

Cons

* Higher larger data and instruction cache pressure than some
  alternatives.
* Must be extended with some load balancing mechanism to allow the use
  of multiple CPU cores.
* Contention for flow-level and/or network layer-related data
  structures may be higher than alternatives.

.. _Static Ingress Load Balancing:

Static Ingress Load Balancing
-----------------------------

The source of work in a data plane is primarily incoming packets.
Packets come in a :term:`NIC` RX queue, or the equivalent in case I/O
is virtualized (e.g., a memif port).

In case only a single NIC, with a single port, and a single RX queue
is available, the run-to-completion application cannot put more than
one :term:`fast path lcore` to work.

In case the system has more ports, more cores may be used. The act of
mapping ports (as the associated RX queue) is a form of static load
balancing. In case all packets come in on one port, all but one of the
system's CPU cores will remain idle.

Static ingress load balancing is a technique which can be used to turn
the basic run-to-completion into a more useful tool.  Both static and
:ref:`dynamic <Dynamic Ingress Load Balancing>` may also be employed
by any of the pipeline models.

Receive Side Scaling
^^^^^^^^^^^^^^^^^^^^

An early implementation of hardware-based ingress load balancing is
:term:`Receive Side Scaling (RSS) <RSS>`, first implemented on early
NICs with multiple RX queues.

The use of RSS requires that multiple RX queues are configured on the
NIC. When RSS is used in an operating system kernel, a one-to-one
correspondens between queue and CPU core is usually maintained, and
the queue-to-core mapping is static. However, in a data plane
application, it may be of value to have more RX queues than cores, to
allow for a limited form of load balancing.

When RSS is employed, the NIC identifies the flow of traffic to which
the packet belongs by examining specific fields in the packet's header
(e.g., the source and destination IP addresses and transport protocol
port numbers).

The RSS function hash (usually a Toeplitz hash) those fields, and uses
the hash value as an index to a RX queue.

RSS can serve in the role of an :term:`atomic <Atomic scheduling>`
type scheduler, where the item of work flow is computed by taking a
hash (e.g., of the packet's source and destination IP addresses).

RSS works best in scenarios with many smaller flows (in the sense of
the RSS classifier). In scenario very few large RSS-level flows, there
is a substaional risk of uneven load distribution, where. In scenarios
where there is only a single input flow, RSS is of no use.

Collisions
""""""""""

Receive Flow Scaling
^^^^^^^^^^^^^^^^^^^^

DPDK Generic Flow
^^^^^^^^^^^^^^^^^

Properties
^^^^^^^^^^

.. _Dynamic Ingress Load Balancing:

Dynamic Ingress Load Balancing
==============================

.. _Pipeline:

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

.. _Scheduling Type:

DPDK Eventdev
^^^^^^^^^^^^^

DPDK Service Cores
==================

.. rubric:: Footnotes

.. [#vpp]
  Such a feature may not only be used to reduce work scheduling
  overhead.  If the buffering also includes some clever reordering of
  packets (i.e., work), so that packets pertaining to the same flow
  are kept together, this will improve temporal locality of memory
  accesses, and also allows for :term:`vector packet processing`.
