..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

.. _Statistics:

Statistics
**********

.. sectionauthor:: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

The topic of this chapter is operational statistics for the data plane
:term:`fast path`.

Data plane statistics provides information on events having occurred
in the data plane application. The same mechanism may also provide a
view into the current state of the data plane. The former is the focus
of this chapter.

The statistics of this chapter serves no purpose for the function the
data plane application provides. In other words, would the statistics
be removed, the user data would still flow through the network as
before.

The statistics are usually a direct result of fast path packet
processing (e.g., counters updated on a per-packet basis). There are
also counters in the slow path (assuming such a component
exists). Often, the slow path and fast path statistics need to be
merged in order to provide a consistent view of the data plane as a
whole.

Slow path statistics are more straight-forward to implement, since the
performance requirements are less stringent, and are out of scope for
this chapter.

Counters
========

In a :term:`SNMP` :term:`MIB`, *counters* are non-negative integers
that monotonically increase, until they wrap upon exceeding some
maximum value (usually 2^32-1 or 2^64-1). A *gauge* is a non-negative
integer that varies within some predefined range.

For simplicity, this chapter uses the term *counter* to include both
what SNMP calls *counter* and what SNMP calls *gauge*. In practice,
the counter is the more common of the two.

In addition to counters, there may also be pseudo statistics
representing the current state of the data plane, rather than a
summary of past events. What type is best used to represent such state
varies; it may be a boolean, an integers or a string. The `Structure
of Management Information (SMI)
<https://www.rfc-editor.org/rfc/rfc2578.txt>`_ - the SNMP MIB meta
model - allows for a number of other types, such as *Unsigned32*,
*TimeTicks* and *IpAddress*. Some of the techniques discussed in this
chapter also apply for managing and presenting state information to
external parties (e.g., the control plane).

There exist a wide variety of network management protocols to access
statistics-type information, where SNMP is just one among many. The
mechanism to access statistics from outside the :term:`fast path`
process, and the data plane application, will be discussed in a future
chapter on control plane interfaces.

Although the available counters and their semantics are likely crafted
to fit some management plane data model (to avoid complex
transformations), the data plane fast path level implementation should
be oblivious to what entities access the data and for what purpose,
and what protocols are being used.

Use Cases
=========

Data plane statistics can be used for a number of different purposes,
including:

* Troubleshooting and debugging
* Node and network level performance monitoring and profiling
* Security (e.g., detecting intrusion attempts)
* In-direct functional uses (e.g., billing)

The vague *in-direct functional use* means that while the counters
have no role in the local data plane function, they still serve a
purpose for the network as a whole. The counters may for example be
used for billing, or as input for automated or manual network
optimization.

Telemetry
---------

Data plane statistics can be used as basis for network *telemetry*.
Telemetry means network nodes provides statistics to a central
location, usually using a push model. This information may then in
turn be used to address one or more of the above-mentioned use
cases. The data plane statistics implementation need not and should
not know if its statistics is being used for telemetry.

Requirements
============

There are a number of aspects to consider, when specifying
requirements for fast path statistics:

* Performance
* Correctness
* Propagation delay
* Time correlation
* Consistency across multiple counters
* Counter Reset
* Writer parallelism
* Reader preemption safety, read frequency, and acceptable read-side
  cost

Performance
-----------

.. _Time Efficiency:

Time Efficiency
^^^^^^^^^^^^^^^

The typical data plane application has a range from a couple of
hundred clock cycles worth of :term:`processing latency` per packet
for :term:`low touch <Low touch application>` applications, up to an
average of a couple of tens of thousands per packet for :term:`high
touch <High touch application>` applications.

Statistics are useful in almost all applications, and supporting a
reasonably large set of data plane counters is essential. In the light
of this, it makes sense to allocate a chunk of the fast path cycle
budget to spend on solving the statistics problem. On the other hand,
the fewer cycles spent on any per-packet task the better. The primary
business value of the data plane comes from the core data plane
function, and only a small minority of the CPU cycles should be spent
on statistics - an auxiliary function.

The number of counters updated per packet will range from somewhere
around a handful for low latency, low touch applications, up into the
hundreds for a complex, multi-layer protocol stack.

One seemingly reasonable assumption is that low touch and high touch
applications spend roughly the same amount of :term:`domain logic` CPU
cycles for every counter they need to update. The more elaborate
logic, the more cycles are spent, and the more there is a need to
account for what is going on, in order to, for example, profile or
debug the application, or the network. [#cyclesvslogic]_ Thus
a high touch application tend to update more counters per packet
than does its low touch cousin.

The next assumption is that no more than 5% of the total per-packet
CPU cycle budget should be spent on statistics, and that roughly one
counter update per 300 CPU cycles worth of domain logic
:term:`processing latency` is expected.

In case these assumptions hold true, the :term:`processing latency`
budget for a counter update is 15 core clock cycles.

Uncertainties aside, this approximation give you an order-of-magnitude
level indication of the performance requirements, and underscores the
need to pay attention to producer-side statistics performance.

As an example of how things may go wrong if you don't, consider an
application with a budget of 5000 clock cycles per packet and a
requirement for an average of 25 counter updates per packet. If the
development team goes down the :ref:`Shared Lock Protected Counters`
path, the statistics feature alone will consume several order of
magnitudes more than the budget *for the whole application*, assuming
the fast path is allocated to a handful of cores or more.

Vector Processing
"""""""""""""""""

If the fast path processing is organized as per the :term:`vector
packet processing<Vector packet processing>` design pattern, the
counter update overhead can potentially be reduced.

For example, if the lcore worker thread is handed a vector of 32
packets by the :term:`work scheduler`, all destined for Example
Protocol-level processing, the ``total_pkts`` need only be updated
once.

.. _Space Efficiency:

Space Efficiency
^^^^^^^^^^^^^^^^

It's not unusual to see data plane applications which track (or
otherwise manage) large amounts of some kind of flows. Such flows may
be TCP connections, HTTP connections, PDCP bearers, IPsec tunnels, or
GTP-U tunnels, or indeed not really flows at all, like :term:`UEs
<UE>` or known IP hosts. Often, it makes sense to have counters on a
per-flow basis. If the number of flows is large, the amount of memory
used to store counter state will be as well. In such applications,
care must be take to select a space efficient-enough statistics
implementation.

There are two sides to statistics memory usage. One is the amount of
memory allocated. The other is :term:`working set size`, and the
spatial (how tightly packed the data is) and temporal locality (how
quickly the same cache line is reused) of the memory used, which
affects :ref:`Time Efficiency`.

The former may impact requirements for the amount of DDR installed on
the system (or allocated to the container), and the amount of
:term:`huge page <Huge pages>` memory made available to the fast path
process.

The latter translate to CPU core stalls (waiting for memory to be read
or written), and thus more CPU cycles required per counter update, and
potentially more CPU cycles for the execution of other part of the
fast path, because statistics processing-generated cache pressure. The
more different counters are updated, the larger the :term:`working set
size` for the application as a whole.

Counters should not be evicted from the last level cache, with
often-used counters held in the L1 or L2 caches.

An issue strictly tied to space efficiency is the amount of DRAM
required to hold the statistics data structures. Depending on how the
counters are organized in memory, only a very small fraction of the
memory allocated may actually be used.

A related question is if memory consumption are required to correlate
with the number of active flows, or if it is acceptable to statically
preallocated statistics memory for the maximum number of flows
supported.

Prototyping may be required to determined how the counter
:term:`working set size` affects performance, both of the statistics
related operations, and other processing. For example, the per-core
counters are usually much more efficient than using a shared
statistics. However, if the shared data structure avoid last level
cache evictions, but it turns out the per-core counter approach
doesn't, the shared approach may surpass per-core counters in
cycle-per-update performance, as well as requiring less memory. Cache
effects are notoriously difficult to prototype, in part since usually,
at the time of prototyping, the real network stack :term:`domain
logic` and the rest of the fast path is not yet in place.

Flow Affinity
^^^^^^^^^^^^^

An aspect to consider when choosing statistics implementation for
per-flow counters is whether or not the :term:`work scheduler` employed
in the fast path maintains flow-to-core affinity.

The affinity need not to be completely static, in the sense a flow
need not be pinned to a lcore worker forever until a system crash or
the heat death of the universe (whichever comes first) occurs.
Rather, it's sufficient if a packet pertaining to a certain flow
*usually* go to the same worker core as the packet preceding it, in
that flow.

Flow affinity reduces statistics processing overhead regardless of
implementation, but applications with shared per-flow counters gain
the most.

Flow affinity makes it likely statistics related data is in a CPU
cache near the core. If, on the other hand, there is no affinity, the
cache lines holding the statistics data may well be located in a
different core's private cache (having just been written to),
resulting in a very expensive cache miss when being accessed. Other
flow-related data follow the same pattern, resulting in affinity
generally having a significant positive effect on overall fast path
performance.

Parallel Flow Processing
^^^^^^^^^^^^^^^^^^^^^^^^

Another important aspect to consider for per-flow counters is whether
or not packets pertaining to a particular flow are processed in
parallel. If a large flow is spread across multiple cores, the
:ref:`shared atomic<Shared Atomic Counters>` and :ref:`shared lock
protected<Shared Lock Protected Counters>` approaches will suffer
greatly, as all worker cores fight to retrieve the same lock, or
otherwise access the same cache lines.

Usually, the data plane fast path includes some non-flow (e.g., global
or per-interface) counters, so the issue with high contention counters
need to be solved regardless.

Update Propagation Delay
------------------------

*Update propagation delay* in this chapter refers to the
:term:`wall-clock latency` between the point in time when the counted
event occurred, until the counter is updated and available to a
potential reader. The result of :term:`domain logic` processing that
caused the counter update (e.g, a packet being modified), and the
counter update itself, are never presented in an atomic manner to the
parties outside the fast path process. This would imply that somehow
the delivery of packets from the data plane and the delivery of
counter updates to the external world could be done atomically.

The counter implementation patterns presented in this chapter, unless
otherwise mentioned, all have a very short update propagation
delay. It boils down to the time it takes for the result of a CPU
:term:`store` instruction become globally visible. This process
shouldn't take more than a couple of hundred nanoseconds, at most.

One way to reason about update propagation delay requirements is to
think about how quickly a user, or any kind of data plane
application-external agent, could retrieve counters which should
reflect a particular packet having been received, processed, or sent.

For example, if serving a control plane request has a practical
(although not guaranteed) lower :term:`processing latency` of 1 ms,
that could be used to set an upper boundary for the counter update
propagation delay.

Organization
============

A common and straightforward way of organizing counters are as one or
more nested C structs. Addressing an individual counter in a C struct
is very efficient, since the offset, in relation to the beginning of
the struct, is known at compile time.

An issue with C structs is that the maximum cardinality of the various
objects (e.g., interfaces, routes, flow, connections, users, bearers)
must be known at compile time. The struct may become very large when
dimensioned for the worst case, for every type of object. Spatial
locality may also be worse than other, more compact, alternatives.

An alternative to structs is to use some kind of map, for example a
hash table or some sort of tree. However, even efficient map
implementations typically has a :term:`processing latency` in the
range 10-100s of clock cycles to access the value of a key.

One radically different approach to managing counters is to maintain
only changes, and not the complete statistics state, in the data plane
fast path application. The aggregation of the changes are either done
in a :term:`data plane control` process or thread, or in the control
plane proper. See :ref:`Shared Counters with Per Core Buffering` for
more on this.

The statistics can either be organized in a global statistics database
for the whole fast path, or broken down per module, or per layer
basis, resulting in many different memory blocks being used.

In case of the global statistics struct type, care must be taken not
to needlessly couple different protocol modules via the statistics
module. On the statistics consumer side on the other hand, it might
make perfect sense to see all statistics, for all the different parts
of the data plane fast path.

Please note that the above discussion is orthogonal to the question
whether or not the statistics data type should be kept in a single
instance (or an instance per module, in the modularized case), or an
instance per lcore.

Separate versus Integrated Counters
-----------------------------------

A design decision to make is if the counters should be an integral
part of other network stack state, or kept separate (possibly together
with statistics from other modules).

For example, consider an application that has a notion of a flow, a
connection, an interface, a port, or a session. It keeps state related
to the processing of packets pertaining to such domain-level objects
in some data structure.

One option is to keep the counter state produced as a side effect of
the network stack :term:`domain logic` processing in the same struct
as the core domain logic state. In case the domain logic state is
protected by a lock, the counter updates could be performed under the
protection of the same lock, at very little additional cost, provided
the counters belongs to that object (as opposed to counters related
to some aggregate of such objects, or global counters).

A potential reader would use the same lock to serialize access to the
counters. This would be an implementation of the :ref:`Shared Lock
Protected Counters` pattern, with per-flow locks. Non-flow counters
would be handled separately.

There are also hybrids, where the statistics is an integral part of
the domain logic data structures, but the statistics synchronization
is done differently compared to accesses of domain object state.

The example implementations in this chapter use a simple,
self-contained, fix size, statically allocated struct, but the various
pattern applies to more elaborate, dynamically allocated data
structures as well.

One way to reduce the amount of memory used (or at least make the
amount of memory used be in relation to the data plane application's
configuration) for systems with high max cardinality, and large
variations in the number of actual objects instantiated, is to use
dynamic allocation, but use fixed-offset accesses to the fields within
those dynamically-allocated chunks of memory. For example, the list of
flows could be a pointer to a dynamically allocated array, instead of
a fixed, compile-time-sized, array.

Large counter data structures should be allocated from :term:`huge
page <Huge pages>` memory, regardless if the statistics struct is
self-contained or spread across many different chunks of memory.

Synchronization
===============

This chapter describes a number of approaches to counter
implementation, with a focus on writer-writer synchronization (i.e.,
synchronization between different lcore worker threads updating the
same counter) and reader-writer synchronization (e.g., synchronization
between the :term:`control threads<Control thread>` and the lcore
workers).

In case where a data plane application has only one lcore worker
thread, or there are multiple lcore workers, but there is no overlap
between what counters the different threads manipulate in parallel,
there is no need for writer-writer synchronization. The only concern
in this case, is reader-writer synchronization, assuming there is a
separate reader thread (e.g., a :term:`control thread`). The solution
to the reader problem can be solved in the same way as is described in
the section for :ref:`Per Core Counters`.

The different patterns described in this section assumes multiple
lcore workers with in part or fully overlapping statistics, and one or
more separate reader threads.

Below are the data structure definition and declarations for the
fictitious Example Protocol (EP). EP has packets and flows, and
counters on both a global level, and on a per-flow basis.

.. literalinclude:: stats_def.h

Needless to say, a real application will have a more counters,
organized into more struct types.

.. _Shared Non Synchronized Counters:

Shared Non Synchronized Counters
--------------------------------

One approach to counter synchronization is to ignore the issue
altogether. The responsible architect might mumble something about
:term:`loads<load>` and :term:`stores<store>` up to 64-bits always
being atomic on modern architectures, and that there's no need to
worry too much about the occasional miscount, since it's "just
statistics".

This approach is disastrous in terms of correctness and performs
poorly - a less than ideal combination.

In order for an lcore worker thread to increment a counter, it will
just call the update function, which in turn will do a normal
C integer addition.

.. literalinclude:: stats_shared_non_atomic_add.c

GCC 10.3.0 compiling for a Cascade Lake generation x86_64 CPU will
generate this code:

.. literalinclude:: stats_shared_non_atomic_add_x86.s
   :language: none

The inc and add instructions act directly against memory, which may
lead the unwary to believe everything is fine so far (which it isn't,
since they lack the "lock" prefix, and thus are non-atomic).

However, the ``vmovdqa`` instruction makes it obvious there is a
problem here. A sequence consisting of a load from memory to register,
a register add, and a store to memory is always a race, in case
multiple core act on the same memory location without employing some
kind of external synchronization.

Here's the same code compiled for the ARMv8-A :term:`ISA`, with the
nice and tidy code generated by Clang 11.0.1:

.. literalinclude:: stats_shared_non_atomic_add_arm.s
   :language: none

This approach, as implemented in the benchmark application described
in :ref:`Performance Comparison` and running on four Cascade Lake
x86_64 cores, loses a whopping ~18% of the global packet count updates
when the fast path is running at max capacity.

.. _Shared Atomic Counters:

Shared Atomic Counters
----------------------

The correctness issue with :ref:`Shared Non Synchronized Counters` can
easily be addressed by using an *atomic* add for the counter producer
and an *atomic* :term:`load` for the consumer side.

In this solution, there is only a single instance of the statistics
struct, accessed by using atomic instructions.

Arithmetic Operations
^^^^^^^^^^^^^^^^^^^^^

Below is the ``ep_stats_update()`` function updated to use atomic
operations:

.. literalinclude:: stats_shared_atomic_add.c

Reading
^^^^^^^

Reading counter values is very straight-forward and :term:`preemption
safe<Preemption safety>`:

.. literalinclude:: stats_shared_atomic_read.c

The use of atomic loads does not impact performance, but does
guarantee that the read is atomic. Naturally aligned :term:`load`
operations up to 64-bit are atomic on all modern CPU
architectures. However, a non-atomic access allows the compiler to
divide a 64-bit load, for example, into two 32-bit loads, which in
turn allows for a counter update to sneak in between the loads,
wreaking havoc with correctness.

Reset
^^^^^

A counter read can be combined with a counter reset by using an atomic
exchange operation.

.. literalinclude:: stats_shared_atomic_reset.c

This approach does not guarantee consistency across multiple, related
counters, since atomicity guarantees are only for a single
counter. For example, a lcore worker may have incremented the session
``pkts`` before the reset was performed by the control thread, and the
``total_pkts`` updates happens after the reset.

Resetting the counters from the reader thread by first reading the
current value with an atomic read, followed by an atomic store of 0,
may lead to counter changes to be lost, since combined, those two
operations are no atomic.
		    
Inter Counter Consistency
^^^^^^^^^^^^^^^^^^^^^^^^^

Shared atomic counters does not guarantee atomicity across multiple
counters. Thus, a reader might see a ``total_bytes`` which has been
updated with the length of a certain packet, but a ``total_pkts``
which value does not reflect that particular packet being processed.

The relaxed C11 memory model (``__ATOMIC_RELAXED``) is used, which
means that the reader might see the updates in a different order than
the :term:`program order`. For counters, this does not create any
additional issues. The reader will prefer to see a consistent view of
the statistics, but performing the updates in a particular,
well-defined order (using a different C11 memory model), does not
help.

Performance
^^^^^^^^^^^

Shared atomic counters perform well if there is little or no
contention on the cache line where the counters reside. For counters
that are updated frequently (e.g., for every packet) by multiple CPU
cores, shared atomic counters performs poorly, because of the costly
cache line ownership transfers incurred.


.. _Shared Lock Protected Counters:

Shared Lock Protected Counters
------------------------------

Another way to extend the :ref:`Shared Non Synchronized Counters`
approach into something that actually works is to do the obvious: add
a lock.  The lock would be used to serialize both writer and reader
access. A per-module statistics lock is used in the example. A more
fine-grained locking (e.g., per-flow locks) strategy is possible, and
depending on how :term:`work scheduling<work scheduler>` is
implemented, this may yield some performance benefits.

If this pattern is used, the statistics struct is supplemented with a
:term:`spinlock`:

.. literalinclude:: stats_shared_lock_protected_def.c

The above example keeps the lock separate from the data, but the lock
may also be added to the struct itself. Co-locating the lock with
often-used (e.g., global) counters may yield a slight performance
benefit. However, if the struct is used elsewhere in the program
(e.g., in :ref:`data plane control <Data Plane Control>`), to pass
around statistics, including the lock may not make sense.

A spinlock is preferred over a POSIX thread mutex, since in the case a
pthread mutex is taken at the point a thread is attempting to acquire
it, the thread is put to sleep by the kernel. A process context is
very costly, and putting DPDK lcore worker threads to sleep usually
doesn't make sense [#sleep]_.

For improved fairness, a ticket lock could be used in place of the
spinlock.

Fine-grained parallelism allows for more parallelism (see
:term:`Amdahl's law`), but the overhead of many acquire and release
operations may turn gains into losses.

Generally, in user space applications, the primary cause of the
performance degradation caused by a lock is related to the length (in
terms of :term:`processing latency`) of the :term:`critical section`
(i.e., how long the lock is held). For the category of
short-transaction systems that data plane applications fall into, the
main issue is usually not critical section length - since it's usually
very short - but rather the lock-related overhead. This overhead
becomes significant in high-contention cases.

So, even though the critical section is just a load, an add, and a
store machine instruction (or something along those lines), the lock
may cause significant overhead.

Arithmetic Operations
^^^^^^^^^^^^^^^^^^^^^

Adding to one or more counters is only a matter of taking the lock,
and perform whatever operations are required, after which the
lock is released.

.. literalinclude:: stats_shared_lock_protected_add.c

Reading
^^^^^^^

Reading counter values is very straight-forward, but *not*
:term:`preemption safe<Preemption safety>`. In case the reader thread
is preempted when it has acquired the lock, but not yet released it,
all the lcore worker threads will eventually be waiting to acquire the
lock, and will continue to make progress only when the reader thread
is re-scheduled and may continue processing and release the lock.

Thus, care must be taken that the :term:`control thread` (or some
other reader) is not preempted, at least not for any long duration of
time.

.. literalinclude:: stats_shared_lock_protected_read.c

The above example includes acquiring and releasing the lock as a part
of the access function. Another option is to have explicit reader lock
and release functions in the ``ep_stats.h`` API. In that case, a
reader may read a large amount of counters, without taking the
overhead for a large number of lock and unlock calls.

Reset
^^^^^

The transactional nature of accessing the statistics that the lock
enables makes supporting counter reset trivial, including maintaining
consistency across multiple, related counters.

One option is to add a separate read-and-reset function, like the
``ep_stats_session_reset`` from the :ref:`Shared Atomic Counters`.

Another option is to use explicit locking for reader (or, for reader
*and* writer) statistics access. In that case, a :term:`control
thread` wanting to reset all, or a subset of, the statistics would:

1. Acquire the statistics lock
2. Optionally read out state of the statistics (prior to reset)
3. Store zeros to the counter variables
4. Release the statistics lock

The implementation could look something like [#reset]_:

.. literalinclude:: stats_shared_lock_protected_reset.c

Inter Counter Consistency
^^^^^^^^^^^^^^^^^^^^^^^^^

A benefit with :ref:`Shared Lock Protected Counters` is the ability to
support consistency cross an arbitrary number of counters. For
example, the ``ep_stats_update()`` function will guarantee that the
total and per-session packet count are updated atomically. A properly
implemented reader will never see data inconsistent in this regard.

Performance
^^^^^^^^^^^

For counters which are incremented for most or all packets,
:ref:`Shared Lock Protected Counters` suffers from the same issue as
:ref:`Shared Non Synchronized Counters` and :ref:`Shared Atomic
Counters`.  Ownership of cache lines that holds the state of such
counters will bounce around between the worker lcores, causing CPU
core stalls and interconnect traffic in the process. For :term:`low
touch applications<low touch application>`, this situation quickly
deteriorate as worker lcores are added to the system. In addition, the
lock forces more costly inter-core synchronization and a full (usually
implicit) memory barrier. This barrier is not present when
:ref:`Shared Atomic Counters` are employed.

:ref:`Shared Lock Protected Counters` is generally prohibitively slow
for counters incremented on a per-packet basis, or similar frequency.

.. _Shared Counters with Per Core Buffering:

Shared Counters with Per Core Buffering
---------------------------------------

The primary performance issue with :ref:`Shared Lock Protected
Counters` is the high contention for the lock, or set of locks, that
protect counters that are frequently updated, from many or all lcore
worker cores.

One way to reduce statistics lock contention and amortize locking
overhead over multiple counter updates, is to introduce a per-lcore
counter update write buffer.

When the data plane fast path application domain logic asks to
increment a counter, no update is performed. Rather, the address of
the counter's memory location, optionally the type (in case different
counters have different data types), and the operand is stored in a
buffer.

When the buffer is full, the statistics framework module will take the
lock and apply all changes. In case the system divides the statistics
memory on a per-module basis, several write buffers may be needed, or
alternatively a single lock may be used to protect the aggregation of
all statistics memory areas.

The number of machine instructions that need to be executed in this
implementation far exceed that of the other patterns described here.
For example, :ref:`Shared Atomic Counters` likely only needs less than
a handful of machine instructions per counter update, compared to the
many instructions needed to store the changes in a buffer, and apply
the changes at a later time.

Nevertheless, this instruction-heavy pattern generally outperforms all
the other shared counter approaches, since it does not suffer from the
cache line ping-pong of the alternatives.

The realization of this pattern is more complex than the alternatives.
Thus, the introduction of a small statistics framework module to is
warranted, to manage this complexity. The API to such a framework
could look something like:

.. literalinclude:: stat_wb.h

The ``stat_wb_schedule_add64()`` function schedules a 64-bit add
operation to be performed at some point. In case the write buffer is
full at the point of the schedule call, it's emptied and all the
buffered arithmetic operations therein are performed. This function
will be called, via a convenience wrapper, by the fast path
:term:`domain logic`, to increment a counter.

This minimal API presented here supports only 64-bit add operations,
but add operations with a smaller integer (or even floating point)
operand could well make sense to include, as well as a
``state_wb_schedule_sub64``, in case a counter is actually a gauge.

In case of gauges, an alternative approach is to change from
``uint64_t`` to ``int64_t``, and while there might exist an add
operation on the API layer, a subtraction is buffered (and applied) in
the form of an add with a negated operand, for improved flush
performance and spatial locality.

``stat_wb_flush()`` forces a flush of the buffer. This may be useful
to do either after a packet, after a batch of packets, or using a
timer, depending on how urgent it is for the updates to be visible to
a potential reader thread within the fast path process.

``stat_wb_try_flush()`` commences a non-blocking attempt to flush the
buffer.

``state_wb_hint_flush()`` is a function which may be called
periodically, for every batch of packets, which will eventually cause
the write buffer to be flush (every Nth call). This may be called for
every batch of packets (or work items) received from the NIC or a
:term:`work scheduler`, in case a flush for every batch is too
expensive. If it's called for zero-sized batches as well, no flush
timer is needed.

Here follows an implementation of the ``stat_wb.h`` API:

.. literalinclude:: stat_wb.c

This implementation uses the DPDK pattern of keeping per-lcore data
structure in static array, sized in accordance to the maximum number
of lcores supported, and using the lcore id as an index into this
array. This is convenient and makes for a clean API, but does prohibit
any non-lcore worker threads from performing counter updates (a
non-lcore thread does not have an lcore id) via the write buffer.

Arithmetic Operations
^^^^^^^^^^^^^^^^^^^^^

The use of the ``stat_wb.h`` API to increment a set of counters could
look like this:

.. literalinclude:: stats_shared_per_core_buffer.c

To slightly improve performance, primarily by amortizing the function
call overhead, and more importantly, the lcore id :term:`TLS` lookup,
across multiple counter updates a multiple-counter update function may
be introduced:

.. literalinclude:: stat_wb_m.c

This function allows incrementing multiple counters in one call. With
this addition to the ``stat_wb.h`` API, ``ep_stats_update`` could be
modified to use the new function:

.. literalinclude:: stats_shared_per_core_buffer_m.c

Beyond the potential performance benefits, this function allows the
updates to be applied atomically against the global ``ep_stats``
counter struct. For this to work, the write buffer buffer must be at
least the size of the largest counter update transaction.

Reading
^^^^^^^

Reading counters is done in the same manner as in the :ref:`Shared
Lock Protected Counters` pattern.

Reader preemption safety is still a concern.

Reset
^^^^^

For resetting counters, the developer is left with the same options as
in the :ref:`Shared Lock Protected Counters` approach.

Inter Counter Consistency
^^^^^^^^^^^^^^^^^^^^^^^^^

In terms providing a consistent view on a set of related counters, or
even all counters, the same opportunities as in :ref:`Shared Lock
Protected Counters` exist.

Performance
^^^^^^^^^^^

:ref:`Shared Counters with Per Core Buffering` is space efficient,
while maintaining fairly good cycle efficiency, both for counters
which are primarily updated by one core, and counters which are
updated by many cores, at high frequencies.

An appropriately sized write buffer is important for characteristics,
and there are a number of forces that pull in different directions:

* A large buffer will increase the :term:`working set size` of the
  application, potentially having a negative impact on performance,
  primarily by causing the eviction of level 1 cache lines used by
  other parts of the fast path.
* If the same counter is repeatedly updated as a part of the same,
  large, write buffer flush transaction, flush performance will
  benefit from a high degree of temporal locality.
* A write buffer which are allowed to grow large will take the lcore
  worker thread a long time to flush, potentially causing an unacceptable
  level of packet latency jitter.
* A small write buffer will cause high contention for the (per-module,
  or global) statistics lock, reintroducing the very issue the write
  buffer is there to solve.
* A large write buffer, combined with the use of the
  ``stat_wb_try_flush()`` function, can be used for mitigation in a
  situation where a thread (e.g., another lcore worker thread, or a
  :term:`control thread`) holds the lock for a relatively long time.

Moving the ``stat_wb`` functions, in particular the add function, to
the API header file may slightly improve the performance [#nogain]_ in
non-:term:`LTO` builds, since then the function may be inlined by the
compiler.

Propagation Delay
^^^^^^^^^^^^^^^^^

Per-core buffering may introduce a noticeable delay between the
counted event occurring, and the time when the counter is updated and
the results are available to a potential reader.

In case the write buffer is not flushed, the execution of the buffered
add operations may be delayed indefinitely.

A simple and often effective model is to force a write buffer flush
for every batch of work items (e.g., packets) a lcore worker
process. In low-load situations, the lcore is usually asked by the
:term:`work scheduler` to process very few packets per batch. In that
situation, the statistics overhead will increase, since the flush
operation is performed with relatively few counter updates
buffered. However, the demand for the fast path to be CPU cycle
efficient is lower during low load. [#nodemand]_ During high load, the
lcore worker will start to experience larger batches, and counter
flushes will contain more operations, reducing per-counter update
overhead.

As an example, assume a fast path application where the processing is
organized as a pipeline, with a per-pipeline stage :term:`processing
latency` of 1000 core clock cycles, running on a CPU operating at 2,5
GHz. When a batch of packets have been processed, the lcore calls
``stat_wb_flush()``. If only a single packet is being processed, the
delay introduced by buffering is at most ~400 ns. If a burst of
packets arrive, an the lcore is being handed 32 packets, the delay
will be less than ~13 us.

.. _Per Core Counters:

Per Core Counters
-----------------

In DPDK, per-core data structures (usually in the form of nested C
struct) are usually implemented by having as many instances of the
struct as the maximum number of support DPDK lcores
(``RTE_LCORE_MAX``), kept in a static array. A DPDK application may
reuse the same pattern, to good effect.

Since DPDK worker threads are pinned to CPU core, and no more than one
worker thread uses a particular core, the per-thread data structure
effectively become a per DPDK lcore data structure.

The DPDK lcore id, numbered from 0 to ``RTE_LCORE_MAX-1``, is then
used as a index into the array of instances. The lcore id may be
retrieved relatively cheaply with ``rte_lcore_id()``.

If the per-core data structures are large, it's better to have an
array of pointers, and only allocated as many as the actual lcore
count, or dynamically on-demand. This will reduce the amount of memory
used, and allow allocation from :term:`huge page <Huge pages>` memory.

This scheme disallows modifying counters from non-lcore worker
threads, but does allow read operations from any thread (even such
that my be preempted).

A data plane application have the option of reusing this pattern
for its own per-core data structures. It may also choose to use
Thread Local Storage (TLS) directly, instead

The example code for this approach uses the same data model and very
similar struct definition as :ref:`Shared Non Synchronized Counters`.

.. literalinclude:: stats_per_core_def.c

.. note:: The ``__rte_cache_aligned`` attribute is crucial from a
   performance perspective. If left out, the data structures for two
   different cores may wholly, or in part, reside on the same cache
   line (i.e., :term:`false sharing`). If frequently-updated counters for
   two different cores are hosted by the same cache line, this shared
   cache line will partly defeat the purpose of using per-core data
   structures. False sharing does not affect correctness.

Arithmetic Operations
^^^^^^^^^^^^^^^^^^^^^

Since the statistics are duplicated across all lcores, no
lcore-to-lcore writer synchronization is required.

.. literalinclude:: stats_per_core_add.c

To guarantee that the counter is written atomically (e.g., to avoid a
scenario where a 64-bit counter is moved from register to memory using
four 16-bit store operations), an atomic store is used. Since there is
only a single writer (i.e., an :term:`EAL thread` or a
:term:`registered non-EAL thread`), the associated load need not be
atomic. More importantly, the whole load + add + store sequence need
not be atomic, and thus a comparatively expensive atomic add (e.g., a
``__atomic_fetch_add()``) is avoided.

A relaxed-memory model atomic store comes without any performance
penalty on all DPDK-supported compilers and CPU architectures. See the
section of reading below for more information on this topic.

Reading
^^^^^^^

To retrieve the value of a counter, a reader needs to take a sum over
all instances of that counter, in all the per-core statistics structs.

A reader may choose to either iterate over all possible lcores (i.e.,
from 0 up to ``RTE_LCORE_MAX``) , or just those actually in use (e.g.,
using the ``RTE_LCORE_FOREACH`` macro from ``rte_lcore.h``). The
read-side operation is usually not very performance sensitive, so it
makes sense to do whatever results in the cleanest code.

Here's an example, using the per-counter accessor function design
style:

.. literalinclude:: stats_per_core_single_read.c

To guarantee that the counter is read atomically (e.g., so that a
64-bit counter is not read with two 32-bit loads), an atomic load
should be used. Non-atomic loads (on the level of the ISA) from
naturally aligned data is always atomic on all contemporary
architectures, and the compiler rarely has a reason to break up the
load into several instructions. However, there is no reason not to use
an atomic load, from a performance perspective.

Atomic loads with the ``__ATOMIC_RELAXED`` memory model does not
require memory barriers on any architectures. It does however imply
that loads of different variables may be reordered. Thus, if ``pkts``
is read before ``bytes`` in the program's source, the compiler and/or
the processor may choose to reorder those load operations, so that
``bytes`` is read before ``pkts``. Usually this is not a problem for
counters, but see the discussion on transactions and consistency.

In the unlikely case atomicity would be violated, the results may be
disastrous from a correctness point of view. For example, consider a
64-bit counter that currently has the value 4294967295 (FFFFFFFF in
hexadecimal). Just as the counter is being read by some control plane
thread, it's also being incremented by one by the owning lcore worker
thread. If the lcore worker store, or the control plane load operation
fail to be atomic, the read may read the least significant 32 bits
from the old value, and the most significant 32 bits from the new
value. What the control plane thread will see is neither 4294967295,
nor 4294967296, but 8589934591.

This phenomena is known as load or store *tearing*.

Inter Counter Consistency
^^^^^^^^^^^^^^^^^^^^^^^^^

The atomic store used by the counter producer thread (e.g., the lcore
worker) is only atomic for a particular counter, and the statistics
may be in a transient state of inconsistency, seen over a set of
different counters, for a short period of time.

For example, if a 1000-byte EP packet is processed, the reader may see
a ``bytes`` counter where that packet is accounted for, but a ``pkts``
which is not yet updated. Similarly, it may read an updated
``total_bytes``, but a not-yet-updated session-level ``bytes``
counter.

Provided the lcore worker thread is not preempted by the operating
system (which should only very rarely happen in a correctly configured
deployment), the time window of the counters being inconsistency is
likely to be very short indeed, but not zero-sized.  If the counters
are updated at a very high rate, the risk for a reader of seeing some
inconsistencies might still be considerable.

The counter state will converge toward a consistent state. This is
often enough, but for application where it is not, and the efficiency
of the per-core counter approach is still required, adding one or more
:term:`sequence counters <Sequence counter>` to protect the
statistics data may be an option.

.. literalinclude:: stats_per_core_seqcount.c

In this solution, the neither the loads nor the stores need be atomic,
since the sequence counter guarantees atomicity over the whole set of
counters.

The introduction of a sequence counter will increase the statistics
overhead, both on the reader, and more importantly, writer side. The
sequence counter requires incrementing a sequence number twice, and a
number of memory barriers. On a Total Store Order (TSO) :term:`ISA`
(e.g., AMD64/IA-64), only an inexpensive compiler barrier is
needed. On a weakly ordered CPU (e.g. ARM), actual barrier
instructions are required.

The reader side will look something like:

.. literalinclude:: stats_per_core_seqcount_read.c

This pattern represents an unorthodox use of a sequence counter, which
is normally used to protect data which changes relatively
infrequently. One issue that may occur, is that the reader will have
to retry many times, known as reader starvation, since the data keeps
changing while it's being read.

Another option to achieve inter counter consistency is to protect the
per-core statistics structure, or structures, with one or more
spinlocks. Since a writer would only contend with a reader for such a
lock, the level of contentions will be very low, and thus the overhead
to acquire the lock will be as well.

Performance
^^^^^^^^^^^

Keeping per-core instances of statistics data structures is generally
the most CPU cycle-efficient way to implement counters.

Add a sequence counter or a spinlock to improve consistency adds
overhead, but from what the :ref:`micro benchmarks <Performance
Comparison>` suggests, it is very small. [#ignorantauthor]_

The largest threat to the viability of the :ref:`Per Core Counters`
approach is space efficiency. The amount of statistics memory grows
linearly with the lcore worker count. This is not an issue in cases
where the amount of statistics is relatively small. For systems
supporting hundreds of thousands or even millions of flows, per-flow
counters are kept, and flows are migrated across different lcore
workers, this approach may prevent the fast path from utilizing many
CPU cores, especially on memory-constrained systems.

.. _Performance Comparison:

Performance Comparison
----------------------

The relative performance of the different approaches to counter
implementation varies with a number of factors.

* Counter update frequency, which in turn is usually depend on the
  per-packet processing latency.
* Counter :term:`working set size` (i.e., the number of counters
  modified).
* The amount of overlap between two or more cores' counter working set.
* Worker core count.
* CPU implementation details (e.g., memory model and cache latency).
  
The benchmarking application simulates a fairly low-touch data plane
application, spending ~1000 clock cycles/packet for domain logic
processing (thus excluding packet I/O). No actual packets are sent,
and the clock cycles spent are dummy calculations, not using any
memory. The numbers measured represent how much the application is
slowed down, when statistics is added. Latency is specified as an
average over all counter add operations.

The counter implementations in the benchmark are identical to the
examples.

The application modifies two global counters, and two flow-related
counters per packet. How many counters are incremented per packet, and
how many are related to a flow, and how many are global as opposed to
per-flow (or the equivalent), varies wildly between applications. This
benchmark is at the low end of counter usage.

In the benchmark, load balancing of packets over cores works in such a
way, that packet pertaining to a particular flow hits the same core,
unless it's migrated to another core. Migrations happens very rarely.
The DSW Event Device is the :term:`work scheduler` used to distribute
the packets. This means there is almost no contention for the cache
lines hosting the two flow-related counters for each of the 1024 flows
in the test.

A real application would likely make heavy use of the level 1 and
level 2 CPU caches, and thus the below numbers are an underestimation
of the actual overhead.

The counters are updated all-in-one-go. In a real application however,
different counters (or sets of counters) will be incremented at
different stages of processing. This makes a difference for the
sequence counter and spinlock-based approaches, in the benchmark the
overhead for the locking/unlocking is amortized over four counter
updates (i.e., the lock is only taken once). In some applications, the
lock may need to be acquired for every counter update, and thus many
times for a single packet.

System under Test Hardware
^^^^^^^^^^^^^^^^^^^^^^^^^^

The "Cascade Lake Xeon" is a server with a 20-core Intel Xeon Gold
6230N CPU. To improve determinism, the Intel Turbo function is disabled,
and all CPU cores run the nominal clock frequency - 2,3 GHz. The
compiler used is GCC 10.3.0.

The "BCM2711 ARM A72" is a Raspberry Pi 4. It is equipped with a
Broadcom BCM2711 SoC, with four ARM Cortex-A72 cores operating at 1,5
GHz. The code is compiled with GCC 9.3.0.

As expected, the variant based sequence counter-based synchronization
suffers somewhat from the weakly ordered memory model requirements'
for barrier instructions.  Surprisingly, on the BCM2711, the per-core
spinlock variant performs as well as the per-core variant which only
uses atomic stores - a fact which the author find difficult to
explain.

DPDK 22.07 was used for both systems.

In both the Raspberry Pi and Xeon server case, the test
application and DPDK was compiled with``-O3 -march=native``.

Benchmark Results
^^^^^^^^^^^^^^^^^

The counter update overhead is expressed in CPU core cycles.

.. csv-table:: Per-counter Update Overhead
   :file: stats.csv
   :header-rows: 1

The different benchmark programs use the various implements the
various patterns described earlier in this chapter, in a manner very
similar to the example code.

.. list-table:: Benchmark Counter Implementation Descriptions
   :widths: 25 75
   :header-rows: 1

   * - Test case name
     - Description
   * - Shared No Sync
     - :ref:`Shared Non Synchronized Counters`
   * - Shared Spinlock
     - :ref:`Shared Lock Protected Counters`
   * - Shared Buffered
     - :ref:`Shared Counters with Per Core Buffering`
   * - Per Core
     - The atomic store variant described in :ref:`Per Core Counters`
   * - Per Core Spinlock
     - The spinlock-protected per core statistics struct variant
       described in :ref:`Per Core Counters`
   * - Per Core Seqcount
     - The sequence counter-protected per core statistics struct
       variant described in :ref:`Per Core Counters`

Device Statistics
=================

Metrics Library
===============

Telemetry Library
=================

.. rubric:: Footnotes

.. [#cyclesvslogic]
   The reverse is not necessarily true. For example, applications that
   copy, encrypt or decrypt the packet payload will have generally
   have high processing latency, but not at the cost of increased
   domain logic complexity or more counter updates.

.. [#sleep]
   Exceptions are, among other, some energy efficiency-related scenarios.

.. [#reset]
   It's unclear to the author what is the appropriate semantics
   for the flow reset should be, in regards to global counters.
   E.g., should the flow's packets be deducted from ``total_pkts``?
		    
.. [#nogain]
   The author's experience is that the performance gains are slight indeed.

.. [#nodemand]
   The reader might ask herself, why there is any demand at all to be
   efficient in low-load situations, as long as you stay under 100%
   CPU utilization. For traditional, always busy-polling DPDK-based
   designs there may be none. However, if the fast path uses CPU power
   management, remaining efficient at low load may allow the
   application to put more worker lcores to sleep, or reduce to an
   even lower core operating frequency, that it could have done
   otherwise.

.. [#ignorantauthor]
   The author does not have any practical experience with the use
   spinlock-protected or sequence counter-protected *per-core*
   statistics in real applications.
