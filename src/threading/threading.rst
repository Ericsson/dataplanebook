..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

.. _Threading:

Threading
*********

Introduction
============

This chapter explains how data plane applications tend to use
operating system threads to get work done, and that in an as
resource-efficient, parallel and low-latency manner as possible.

The DPDK approach to threading is by no means unique to DPDK. Similar
patterns for how to distribute fast path processing across threads are
used in other data plane platforms and application, such as
:ref:`ODP`, :term:`Open vSwitch` and fd.io :term:`VPP`. To the
author's knowledge, there are no alternatives that are able achieve
data plane type characteristics on top of a general-purpose operating
system kernel. [#alternatives]_

The DPDK threading model provides excellent performance
characteristics, but at the cost of somewhat difficult-to-deploy and
difficult-to-understand applications, among other things.

The DPDK threading model is a source of much confusion, and many a
misunderstanding and surprise. More generally, the DPDK-stipulated
architecture may seem complex, awkward, and comes with a set of
drawbacks. It does not make much use of kernel or C library-level
services, such as functions for load balancing and concurrency,
accelerator and network I/O hardware abstraction and associated
drivers, memory management, time and timer management, and thread
synchronization.

In this book, the terms :term:`parallelism` and :term:`concurrency`
are used with two distinctly different meanings. Please refer to the
glossary definitions.

How DPDK-based applications use threads to achieve
:term:`parallelism`, and how threads usually are *not* the vehicle for
:term:`concurrency` is different from how things are often done in for
example Java, Go or C++ [#cppthreadoptions]_ applications.

Equally different, and in part the result of, or the reason for, the
DPDK threading model, is the choice of synchronization primitives,
inter-thread messaging mechanisms, and work scheduling in a DPDK-based
application, compared to an application designed in the UNIX
tradition.

This chapter leaves out :term:`concurrency` and :term:`work scheduling
<Work scheduler>`. This topic will be covered in a separate chapter.

UNIX Networking Application Architecture
========================================

This section will describe the traditional UNIX approach to networking
applications, employed to craft a data plane fast path, when it is
written. [#unix]_

DPDK Threading Model
====================

Overview
--------

The recipe for building and deploying an application adhering to the
DPDK threading model, in its most basic form, is roughly as follows:

* Determine which of the system's :term:`CPU cores <CPU core>` will be
  dedicated to the DPDK application process.
* :term:`Clear <Core isolation>` the application-owned cores from as
  many user space threads, and kernel space threads, top and bottom
  half interrupt handlers as possible.
* At the time of DPDK application invocation, inform the application
  which cores to use.
* In the application's ``main()`` function, call DPDK's
  `rte_eal_init() <https://doc.dpdk.org/api/rte__eal_8h.html>`_,
  which, among other things, spawns as many :term:`EAL threads <EAL
  thread>` as there are application-owned :term:`CPU cores <CPU core>`
  [#mainthread]_, and :term:`pin <Processor affinity>` each thread to
  one of the cores.
* Have the fast path packet processing EAL threads run continously
  and indefinitely, polling NIC receive queues and other sources of
  work.

This section will dwell into the details of this model, its pros and
cons, and its variants and extensions.

Benefits and Drawbacks
^^^^^^^^^^^^^^^^^^^^^^

In summary, the DPDK threading model has the following benefits:

* Arriving packets (and other :term:`items of work <Item of work>`)
  are dealt with quickly, and without incurring the cost of context
  switches or system calls in the process.
* Packets may be passed between cores forming a pipeline, using
  low-overhead, shared memory-based message passing, without
  requiring any context switches and system calls.
* Processing of a packet (or some other item of work) is generally not
  interrupted. In particular, an :term:`EAL thread` is never preempted
  and replaced with a peer :term:`EAL thread`. This, in turn, means:

  * Packet processing :term:`jitter` is kept at a minimum, and soft
    real-time deadlines can be met.
  * Efficient and :term:`thread-safe <Thread safety>`, but
    :term:`non-preemption safe <Preemption safety>`, shared data
    structures may be used.

The drawbacks for the basic model are, in short:

* The busy-polling EAL threads continuously use all CPU time available
  leading to:

  * Poor energy efficiency at low or medium load, since used cores are
    constantly kept at maximum operating frequency and out of any
    sleep states.
  * Reduced performance for :term:`SMT` systems, since an EAL thread
    will use significant :term:`physical core` resources, even when
    no useful work is performed (i.e., being a :term:`noisy SMT
    neighbour <Noisy neighbour>` to another thread running on the same
    physical core).

* The use of :term:`core isolation` leads to cores allocated to the
  DPDK application cannot be shared (pooled) with other applications
  in the system, which in turn leads to worse overall hardware
  resource utilization.
* It is not possible to dynamically scale up application to use more
  CPU cores than it has EAL threads.
* Dynamically scaling down to fewer cores requires discontinuing the
  use of certain EAL threads, which generally is supported by DPDK
  libraries and device drivers, but may pose a challenge for the
  application itself, which potentially rely on per-lcore objects
  (e.g., timers, :term:`RCU` memory reclamation, or event device and
  ethernet device ports/queues), which cannot be left unattended.
* For :term:`CNFs <CNF>`, core isolation somewhat complicates
  container-level scheduling.

DPDK's basic threading model can be extended to mitigate, or even
eliminated, these problems. Such improvements will be covered in this
chapter, and other future chapters, for example a chapter on power
management.

EAL Threads
-----------

The :term:`EAL thread <EAL thread>` is the workhorse of DPDK-based
data plane fast path applications.

An EAL thread is an operating system thread created and managed by the
DPDK's core platform library - the :term:`EAL`. All EAL threads are
spawned at EAL initialization, and lives throughout the lifetime of
the DPDK process.

Logical Cores
^^^^^^^^^^^^^

In general, the term :term:`logical core` refers to an entity, usually
a piece of hardware, behaving like a :term:`CPU core` from the point
of view of the software program it is running. The hardware-software
interface of which the logical core is a key part is called an
:term:`instruction set architecture (ISA) <ISA>`.

A logical core may be realized as a :term:`hardware thread <Hardware
threading>`, a :term:`full core`, or in exceptional cases, a
software-emulated core - all of which are functionally equivalent,
from a software point of view.  [#logicalcoreperformance]_ The
seemingly useful term logical core seems rarely used, as does a
synonym: :term:`virtual core`.

When the term :term:`logical core` is used in a DPDK context - usally
abbreivated to :term:`lcore` - it means something related, but
distinct from the generic, hardware-level concept.

The DPDK lcore is a only a different name for an :term:`EAL
thread`. The reason for the lcore designation is that an EAL thread is
usually :term:`pinned <Processor affinity>` to one particular
:term:`logical core`, dedicated to its use.

When this book uses the abbreviated *lcore* form, the DPDK meaning of
the word is implied.

`<rte_lcore.h> <https://doc.dpdk.org/api/rte__lcore_8h.html>`_ is the
primary API for lcore-related operations, such as EAL thread iteration
and status queries.

Main and Worker Lcores
^^^^^^^^^^^^^^^^^^^^^^

As a part of the :term:`EAL` initialization (i.e., the
``rte_eal_init()`` call), the calling thread is repurposed as an
:term:`EAL thread`, and designated the :term:`main lcore`.

The EAL default is to assign the lowested-numbered lcore the main
lcore role. The default may be overridden with the ``--main-lcore``
:term:`EAL parameter <EAL parameters>`.

By default, EAL, during its initialization, spawns one operating
system thread for each :term:`logical core` in the ``main()`` thread's
original :term:`processor affinity mask <Processor affinity>`, beyond
the first. All such EAL threads are a :term:`worker lcores <Worker
lcore>`.

For example, if a DPDK application is invoked with 12 cores in the
``main()`` function thread's :term:`affinity mask <Processor
affinity>`, ``rte_eal_init()`` will, barring any :term:`EAL
Parameters` saying otherwise, create 12 EAL threads. One of these EAL
threads is the main lcore, using the caller's operating system thread,
and the 11 others are worker lcores, each associated with a newly
created operating system thread.

The number of worker lcores may be, and usually is, reduced compared
to the default. For more information, see the :ref:`Lcore Affinity`
and :ref:`Core Allocation` sections.

The ``RTE_LCORE_FOREACH()`` and ``RTE_LCORE_FOREACH_WORKER()`` macros
may be used to iterate over both the main and the worker lcores, or
just the worker lcores, respectively.

Progammatically or by using :term:`EAL Parameters`, certain worker
lcores may be asked to take the role of a :term:`service lcore`. See
the :ref:`Service Cores` section for details.

Worker Launch
"""""""""""""

EAL threads serving as :term:`worker lcores <Worker lcore>` are
assigned tasks using the `<rte_launch.h>
<https://doc.dpdk.org/api/rte__launch_8h.html>`_ API.

A common pattern is launch a more-or-less permanently running
function, and then deal with more fine-grained work scheduling by
other means (e.g., a combination of DPDK event devices, DPDK timers
and DPDK ethernet devices). See the future chapter on :ref:`Work
scheduling` for more information on this subject.

After having finished initializing the DPDK platform and application,
and launched all workers, the :term:`main lcore` itself may take on
some long-runnning fast path task.

.. literalinclude:: launch.c

In this very rudimentary example all EAL threads are employed for fast
path work.

The basic structure of this program shares a resemblance with one that
calls into POSIX thread API. An important difference that
``rte_eal_remote_launch()`` doesn't launch a thread, in the sense of
creating it, but rather only assigning an :term:`item of work`, in the
form of a function to execute, to an already-existing thread.

One way to see DPDK worker lcores, is as a fixed-sized thread worker
pool, which works on one task at a time, and where the assignment of
tasks is directed at a particular worker, putting the burdon of load
balancing the caller. However, in DPDK, the task is often of the
never-ending nature, only terminating when some :term:`control thread`
says it's time for an orderly shut down of the application.

Fast Path Lcores
^^^^^^^^^^^^^^^^

Most or all EAL threads in most DPDK fast path application are
assigned tasks with demanding throughput requirements, paired with
requirements to keep latency and :term:`jitter` below some upper
bound. This book will refer to such cores as :term:`fast path lcores
<Fast path lcore>`. A DPDK lcore in service in any type or role (i.e.,
:term:`main lcore`, :term:`worker lcore`, or :term:`service lcore`)
may fit this description.

One might argue *real-time lcore* would be a more suitable designation
for such cores, considering the soft real-time characteristics
requirements prevalent in the data plane domain. However, this term
may had lead the unwary to believe it somehow implied the use of
real-time scheduling policies, or the ``CONFIG_PREEMPT_RT`` real-time
Linux kernel patches. Fast path lcores generally do not depend on
neither of those. In addition, the archetypal hard real-time system is
not designed to operate under the kind of system load the fast path
application has to endure. That fact is reflected in the design in
Linux' RT scheduling policies, discussed in the ref:`Real Time
Scheduling Policies` section.

The DPDK documentation doesn't have a word for fast path lcores,
although in at least one instance *DPDK processing threads* and
*forwarding threads* is used. [#noterm]_

.. _Lcore Affinity:

Lcore Affinity
^^^^^^^^^^^^^^

By default, the :term:`processor affinity` of the :term:`EAL threads
<EAL thread>` is set is such a manner, that each EAL thread may only
be scheduled on one :term:`logical core`, and that each logical core
has exactly one EAL thread pinned to it. In other words, there's a
one-to-one relationship between a DPDK lcore and the underlying
hardware logical core.

The set of CPU cores allocated to an application may be, and usually
is, reduced by using :term:`EAL command-line parameters <EAL
parameters>` (i.e., ``-c`` or ``-l``). The default is to use all cores
available in the ``main()`` function thread's original affinity
mask. See the :ref:`Core Allocation` section for more on this
subject.

Most DPDK-based fast path applications are designed with the
assumption of a one-to-one relationship between EAL thread and
:term:`logical core`. [#1ton]_

Peer Preemptable EAL Threads
""""""""""""""""""""""""""""

By using the ``--lcores`` :term:`EAL Parameter <EAL Parameters>`, the
default 1:1 mapping between EAL threads and logical cores may be
changed into a M:N relationship. This flag may be used to create
a number of different process scheduling scenarios.

``--lcores`` may be used to include a particular CPU core in the
affinity mask of more than one EAL thread. For example, a 2:1 mapping
may be used, where two lcores are mapped against one logical core.

In case such EAL threads are configured with a normal time-sharing,
preemptible multitasking, scheduling policy (e.g., ``SCHED_OTHER`` on
Linux), which is the default, they do not qualify as
:term:`non-preemptable <Non-preemptable thread>`.

Preemptable EAL threads suffer severe limitations in terms of what
kind of latency characteristics they deliver, and generally can't be
used for fast path packet processing. In addition, they cannot safely
use many DPDK APIs. See the section on :ref:`Non-preemption Safe
APIs`.

.. _Cooperative Multitasking:

Cooperative Multitasking
""""""""""""""""""""""""

:term:`Peer preemptable EAL threads <Peer preemptable EAL thread>`
coexisting (i.e., are being scheduled) on by the same CPU core may be
turned non-preemptable provided they all have the ``SCHED_FIFO``
scheduling policy, the same priority, and use ``sched_yield()`` to
yield the CPU in situations when it is safe to do.

Cooperative multitasking allows for the use of EAL threads for the
purpose of concurrency (e.g., to run different modules), at the cost
of context switches and the significant complexity introduced by the
use of a combination of high CPU utilization and real-time scheduling
policies. See the section on :ref:`Real Time Scheduling Policies` for
more information on the latter.

.. _Floating EAL Threads:

Floating EAL Threads
""""""""""""""""""""

The ``--lcores`` :term:`EAL Parameter <EAL Parameters>` may be used to
instruct the :term:`EAL` to include more than one CPU core in one or
more EAL threads' :term:`affinity masks <Processor affinity>`.

Having an EAL thread :term:`floating <Floating thread>` on two or more
*dedicated* cores makes very little sense, since a single thread
cannot can't use more than one core at a time. This scenario will be
left aside.

The other use case for floating EAL threads is to have them overlap,
either with other EAL threads, or with non-EAL threads, from the DPDK
application process, or some other process. In such a scenario, the
kernel's process scheduler attempts to load balance all threads across
the cores available to each thread.

Floating threads with a ``SCHED_OTHER``-type scheduling policy will
suffer from the kernel's inability (or rather, unwillingness) to
migrate such threads quickly from one core, to another.

This behavior, which can be tweaked by means of kernel runtime
configuration (e.g., setting a low migration cost), leads to
situations where there are runnable floating EAL threads are left
waiting, even though there is an idle CPU core in its affinity
mask. See the :ref:`Process Scheduler` section for more information.

If the kernel is configured to quickly migrate ``SCHED_OTHER``
threads, or if a real-time scheduling policies, which perform
immediate rebalancing, is used, the fast path application might suffer
from the very thing the migration cost concept of the ``SCHED_OTHER``
policy is trying to address: there's a cost associated to migration,
primarily in the form of cache misses in core-private CPU caches.

The floating threads approach will have less of a disastrous outcome
if the EAL threads avoid busy-polling, either by using interrupts
relayed over a file descriptor from the kernel, or by calling
``usleep()`` (or similar) at times they have nothing to do. In this
scenario, they are less likely to be considered batch-type thread, and
less likely to be interrupted by threads that are considered
interactive.

Floating and preemptible EAL threads only make sense under very
specific circumstances (e.g., in the context of functional tests).
The author has trouble imaging a real-world production scenario in
which floating EAL threads provides a net benefit.

Lcore Identifier
^^^^^^^^^^^^^^^^

Each DPDK :term:`EAL thread` and each :term:`registered non-EAL
thread` is assigned process-unique non-negative integer identifier.
Lcore id allocation is a task of the DPDK :term:`EAL`.

This *lcore id* is in the range from zero to ``RTE_MAX_LCORE-1``
(inclusive).

The DPDK lcore id and the kernel-level CPU id (e.g., :term:`CPU`
number in Linux) usually, but not always, have the same value, for the
same :term:`logical core`.

The lcore id to kernel CPU id mapping may be controlled by means of
DPDK :term:`command-line arguments <EAL parameters>`.

Thread Local Data
^^^^^^^^^^^^^^^^^

The :term:`EAL` keeps lcore id and other tightly coupled and
frequently accessed EAL thread-related data in :term:`thread-local
storage <TLS>`. Such data includes the EAL thread's :term:`NUMA node`
and thread's :term:`processor affinity` at the time it was
created. This caching scheme implies that EAL considers the affinity
and :term:`NUMA` placement as an constant invariants across
the DPDK process life cycle.

In addition, many DPDK libraries and :term:`PMDs <PMD>` keep per-EAL
thread data, usually in the form of a static array indexed by the
:term:`lcore id`.

The DPDK EAL, library and driver per-EAL thread data is also kept for
:term:`registered non-EAL threads <registered non-EAL thread>`.

Thread Count
^^^^^^^^^^^^

DPDK has a compile-time upper bound for the number of concurrent EAL
threads, controlled by the ``RTE_MAX_LCORE``. This limit may be
increased, but DPDK reliance on per-thread data effectively prevents
very large numbers of EAL threads. Usually, ``RTE_MAX_LCORE`` is set
higher than, but still the same order of magnitude as, the highest
core count system the build is targeting. On POWER and x86_64 builds,
for example, the compile-time default for ``RTE_MAX_LCORE`` is 128.

The ``RTE_MAX_LCORE`` limit must be set to also accomodate any
:term:`registered non-EAL threads <Registered non-EAL thread>`.

The ``rte_lcore_count()`` function may be used to retrieve the actual
number of lcores. Note however that this count also include
:term:`registered non-EAL threads <Registered non-EAL thread>`, if any
such have been created.

Invitation Only APIs
^^^^^^^^^^^^^^^^^^^^

Certain DPDK APIs and certain kind of DPDK synchronization primitives
only be safely used by threads with certain properties.

.. _Lcore Id Only APIs:

Lcore Id Only APIs
""""""""""""""""""

Threads equipped with a :term:`lcore id` posses special powers, in the
sense there are DPDK APIs in where such threads get a preferential
treatment, or indeed may be that only one that can safely use them.

For example, the rte_rand() function of the `<rte_random.h>
<https://doc.dpdk.org/api/rte__random_8h.html>`_ API is only :term:`MT
safe` if called from a :term:`lcore id`-equipped thread.

Only :term:`EAL threads <EAL thread>` and :term:`registered non-EAL
threads <Registered non-EAL thread>` have lcore ids.

For efficiency reasons, DPDK drivers or a libraries often employ
per-lcore data structures, usually in the form of an array indexed by
the lcore id. Threads without lcore ids may either be not be allowed
to call the API, may call it, but suffer worse performance or a lack
of MT safety guarantees, forcing external synchronization, or the
library may fall back to thread-local storage.

Another reason for an API to require an lcore id is a mere matter of
API design. For example, in the original `<rte_timer.h>
<https://doc.dpdk.org/api/rte__timer_8h.html>`_ API, a timer wheel was
addressed by means of the lcore id of the managing thread.

.. _Non-preemption Safe APIs:

Non-preemption Safe APIs
""""""""""""""""""""""""

DPDK includes many synchronization primitives (e.g., the
:term:`spinlock` and the :term:`sequence counter`) and
:term:`thread-safe <Thread safety>` low-level data structures (e.g.,
the default ring) which are not safe to use for threads which may be
preempted. In addition, many higher-level API library and driver
implementations (e.g., the `timer library
<https://doc.dpdk.org/guides/prog_guide/timer_lib.html>`_, `eventdev
<https://doc.dpdk.org/guides/prog_guide/eventdev.html>`_, and `service
cores framework
<https://doc.dpdk.org/guides/prog_guide/service_cores.html>`_) use
such constructs in their implementation, and thus also aren't
:term:`preemption safe <Preemption safety>`.

The result of a thread calling a non-preemption safe API, and then
being interrupted by the kernel during the call, is that it may
interfere with the forward progress of (i.e, block) other
threads. Such a situation is generally not a threat to functional
correctness of the application, but may have disasterous affects on
throughput and latency - in particular latency jitter. Simply put,
:term:`preemption safety` is to performance what :term:`thread safety`
is for correctness.

Especially harmful is a case where a running thread suffers an
involuntary context switch, and is replaced with a thread which in
turn is *busy-waiting* for previous thread to produce some result.
This waiting normally continues until the next involuntary preemption,
or the first thread being migrated to a new CPU core, either of which
may take 10s to 100s of milliseconds. [#ladawait]_ This scenario is
sometimes referred to as *lock holder preemption*. This book uses the
term :term:`peer preemption <Peer preemptable EAL thread>`.

Using spinlocks and other non-:term:`wait-free <Wait-free algorithm>`
constructs in a user space program may seems like a poor design, but
under the right conditions, this is the most performant solution. With
the DPDK default lcore deployment and isolated cores, under the
assumption that the :term:`critical section` of a lock (or the
equivalent) is very short, busy-waiting is less costly than issuing a
system call (e.g., ``select()``) and putting the thread into sleep
waiting for some file descriptor to become active, like a non-DPDK
application would have done. This is especially true for
:term:`high-contention <Lock contention>` cases. This subject will be
explored further in the future :ref:`Synchronization` chapter.

:term:`EAL threads <EAL thread>` are normally spared from preemption,
and in particular completly safe from :term:`peer preemption <Peer
preemptable EAL thread>`.

There is at least one other way to avoid preemption, than the method
normally employed by the DPDK threading model. It may be perfectly
safe for a thread configured with a real-time scheduling policy to use
share non-preemption safe data structures with a set of fast path
lcores. See also the section on :ref:`Cooperative Multitasking` for
more information.

Historically, DPDK API documentation has been lacking in the area of
specifying multi-thread safety, preemption safety, and related
concerns, such as signal handler safety.

Non EAL Threads
---------------

In a DPDK-based fast path process, not all threads are :term:`EAL
threads <EAL thread>`. Unsurprisingly, the DPDK documentation refers
to such threads as :term:`non-EAL threads <Non-EAL thread>`, and this
book will stick to that term.

More unexpected is that the EAL may be the source of such non-EAL
threads (e.g., the :term:`control thread` for interrupt handling).

Unregistered Non EAL Thread
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Threads that are created using non-DPDK API calls (e.g., direct or
indirect calls via non-DPDK libraries to ``pthread_create()``) are
referred to as :term:`unregistered non-EAL threads <Unregistered
non-EAL thread>`.

Threads created prior to the ``rte_eal_init()`` call inherit get the
``main()`` function's thread :term:`affinity <Processor affinity>`,
and after that will get the main lcore's affinity, which usually means
they are pinned to a main lcore's CPU core. The affinity settings for
both those cases are less than ideal, since they result in a thread
floating (migrating) into the main and worker lcores' CPU cores,
threatening their :term:`preemption safety`.

Unregistered non-EAL threads are best off having an affinity which
either coincide with that of DPDK :term:`control threads <Control
thread>`. An alternative approach is to not employ the main lcore as a
:term:`fast path lcore`, but instead use it for running various
control threads, in conjunction with the :term:`main lcore`'s thread.

Threaded Libraries
""""""""""""""""""

Special care need to be taken for threads created by a non-DPDK
library, linked to the DPDK application. A particularly troublesome
sub category is libraries that spawn threads "under the hood", without
the application's knowledge and consent, and where the POSIX thread id
is unavailable to the application.

Preferably, the use of such libraries should be avoided. Even outside
the context of the data plane, background threads in generic libraries
is generally a sign of poor library design.

If such libraries cannot be avoided, care must be take to assure that
threads created by it on the behalf of the application receives the
appropriate affinity settings and scheduling policy, or otherwise is
made to not interfere with the :term:`fast path lcores <Fast path
lcore>`.

Registered Non EAL Thread
^^^^^^^^^^^^^^^^^^^^^^^^^

An :term:`unregistered non-EAL thread <Unregistered non-EAL thread>`
may :term:`register <Registered non-EAL thread>` in the :term:`EAL`,
by calling ``rte_register_thread()``.

One of the powers granted to a registered non-EAL thread is that of
any holder of an :term:`lcore id`: access to DPDK APIs that require
the calling thread to have such an identifier. See the :ref:`Lcore Id
Only APIs` section for more information.

A registered non-EAL thread may use ``rte_lcore_id()`` to retrieve
their lcore id, in the same manner as an EAL thread would.

A registered non-EAL thread is generally *not* considered an lcore
(i.e., an EAL thread), and the ``RTE_LCORE_FOREACH()`` loop macro will
exclude registered non-EAL threads.

However, the EAL-internal represention of a registered non-EAL thread
is an instance of the EAL thread data structure, but with a role
attribute set to ``ROLE_NON_EAL``.

A registered non-EAL thread is a disabled EAL thread, in the sense
``rte_lcore_is_enabled()`` returns false. It serves in the
``ROLE_NON_EAL`` role, if asked by ``rte_eal_lcore_role()``.

The lcore count produced by ``rte_lcore_count()`` *does include*
registered non-EAL threads.

There is no way for a registered non-EAL thread to receive launched
tasks (i.e., it cannot be the subject of a ``rte_launch_task()``
call).

For a discussion on why DPDK's thread-related terminology is not
internally consistent, see :ref:`A Terminology Side Note`.

Control Threads
---------------

Using ``rte_ctrl_thread_create()``, an application may spawn what DPDK
calls a :term:`control thread`.

A DPDK control thread starts its life as an :term:`unregistered
non-EAL thread`. The EAL sets control thread's :term:`affinity mask
<Processor affinity>` to that of the process original ``main()``
sthread's affinity mask, at the time it initialized the EAL, but with
all cores used for running EAL threads removed.

An alternative to using this DPDK convenience function for thread
creation, is to rely on standard ``libc`` facilities (e.g.,
``pthread_create()``).

If the control thread needs access to DPDK APIs requiring the caller
to possess a :term:`lcore id`, the control thread needs to
:term:`register <Registered non-EAL thread>`.

Refer to the section on :ref:`Data Plane Control` for more information
on what role a DPDK control thread may serve, from a functional or
architectural perspective. Note that DPDK itself, and the application,
may employ DPDK control threads for other purposes as well.

Control and Fast Path Thread Interaction
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:term:`Control threads <Control thread>` may need to interact with
:term:`fast path lcores <Fast path lcore>` for a number of reasons,
such as affecting changes in configuration or retrieving various
state information (e.g., counters).

Each of the :term:`EAL threads <EAL thread>` normally runs on an
:term:`isolated core <Core isolation>`, dedicated for its use. Such
luxuries cannot always be afforded mere control threads, which then
often instead are deployed :term:`floating <Floating thread>` on a set
of cores, shared by other threads, leaving them :term:`preemptable
<Preemptable thread>`.

This section covers two ways to deal with this problem, in an resource
efficient and safe manner: either make them non-preemptable, or use
preemption safe ways to interact with the fast path lcores.

See also the :ref:`Non-preemption Safe APIs` section.

Non Preemptable Control Threads
"""""""""""""""""""""""""""""""

A straight-forward way to deal with the inability of a preemptable
control thread to safely interact with :term:`fast path lcores <Fast
path lcore>` is make them :term:`non-preemptable <Non-preemptable
thread>`.

The obvious, and maybe also obviously too costly, way is to use a
per-control thread :term:`isolated core <Core isolation>`.

Another option is to configure the control threads with real-time
scheduling policy. In case multiple control threads shares the same
CPU core, they should be configured with ``SCHED_FIFO`` and the same
static priority, and cooperate in the same manner as described for EAL
threads, described in the :ref:`Cooperative Multitasking` section.

Barring any higher-priority, real-time-priority, long-running threads
scheduled on the same core, the ``SCHED_FIFO``-equipped control
threads will qualify as non-preemptable. See :ref:`Non-preemption Safe
APIs` for more information on this subject.

Like always when absolute-priority, real-time scheduling policies are
used, care must be taken not to starve other threads, in particular
kernel threads bound to particular cores. See the section on
:ref:`Real Time Scheduling Policies` for more discussion on the use of
real-time scheduling policies in data plane applications.

Preemptable Control Thread
""""""""""""""""""""""""""

Preemptable control threads may be safely used, with careful design of
the interaction with the :term:`EAL threads <EAL thread>`, with the
use of preemption-safe means of communication.

For simple data types, accessed in a non-transactional manner, C11
atomics, primarily in the form of atomic loads and stores, may be
used. Typically, the control thread would atomically store
configuration updates, and atomically load state, statistics, trace
events, and other information from the fast path data structures.

For updates larger than 64 bits, relying machine-level atomic
instructions may not be possible. In that case, a sequence lock may
look like a temption option, for data which is often-read from the
fast path, and only occasionally written by the control
thread. However, a sequence lock is not preemption-safe on the writer
side, although the criticial section is usually small, so depending on
application it may be an acceptable level of risk. The risk being one
or more :term:`fast path lcores <Fast path lcore>` being unable to
make progress. Sequence lock does provide a means to abandoned a
failed read transaction, which may be used to allow the thread to make
progress, provided it keeps copy of the old data, and using the old
data doesn't threaten application correctness.

:TERM:`Read-copy-update (RCU)<RCU>` may be used to update more
elaborate configuration or other data structures, with a set of
dependent values.

By default, DPDK rings are not preemption-safe, but when operated in
``MP_RTS/MC_RTS`` or ``MP_HTS/MC_HTS`` mode, they are. Such rings a
good option for messaging. A clean, and conservative design is to
interact between the control threads and the fast path lcores only by
means of messaging. It may results in more code, and lower
performance, for control plane signaling-intesive applications. Such a
ring may also be used as a relay, or a very basic deferred-work
mechanism, just passing a function pointer to a :term:`fast path
lcore`, asking it to perform some action (e.g., an configuration
update) in a preemption-safe context.

Preemptable control threads acquiring locks, whether spinlocks,
mutexes or some other kind of blocking mutual exclusion mechanism,
shared with fast path lcores causes issues similar that of
:term:`priority inversion` in a real-time system.

Interaction between preemptable control threads and EAL threads may
occur in a obvious way. For example, the control thread takes a
spinlock to update some table, read some packet counter value
incremented by the fast path lcores, or attempts to dequeue elements
from a ring which an EAL thread may have written to.

There are also more subtle cases, where the interaction is much less
obvious. For example, it may happen as a side affect of calls to
shared libraries. A call to ``malloc()``, for example, may result in a
POSIX mutex lock being taken by the caller. If the holder is
preempted, and a fast path lcore attempts to allocate memory, it will
be put to sleep and not be awaken by the kernel until the mutex is
released.

More in-depth discussion on this subject will appear in the future
:ref:`Synchronization` chapter.

EAL Control Threads
^^^^^^^^^^^^^^^^^^^

The EAL creates a number of DPDK control threads for internal
use. They are control threads in the DPDK sense only, not in the sense
described in :ref:`Data Plane Control`. They are usually employed in
an auxaliary role, and not in the form of a control plane agent.

Interrupt Thread
""""""""""""""""

Interrupts cannot be directly received by a user space
process. However, there are ways for the kernel to relay this
information to a process.

At time of initialization, :term:`EAL` spawns a :term:`control thread`
for the purpose of relaying this information to other threads in the
application.

Other Control Threads
"""""""""""""""""""""

There may be other DPDK control threads of a DPDK-internal origin
(i.e., which are not the direct result of an application calling
``rte_ctrl_thread_create()``.

Some examples of modules employing control threads:

* vhost library
* eventdev RX adapter
* vdpa driver
* dlb2 driver

.. _Core Allocation:

Core Allocation
----------------

A :term:`network function` hosting a DPDK-based fast path application
must include an entity which decides which of the available CPU cores
(and the :term:`logical cores <Logical core>` they present to the
software) are to be dedicated, or otherwise used, by the fast path
process (as well as other processes). This CPU resource manger may be
a part of a more generic resource manager function, which also manages
I/O devices, accelerators and memory. Those subjects are left to the
future chapters (e.g., :ref:`Ethernet Devices` and :ref:`Memory
Management`).

The CPU resource manager described here takes on a task similar to,
but much simpler than, that of a Kubernetes scheduler in a Kubernetes
cluster, but only internally, in the :term:`network function`.

The CPU resource manager are best off left outside the fast path
application itself, since it deals with system (i.e., :term:`network
function`-wide) concerns. The data plane fast path is likely not the
only user of CPU resources. Knowledge about other application
processes, their resource requirements, and the mutual priority
generally do not belong in fast path application, or even the data
plane.

Core allocation also includes steps to assure that the appropriate CPU
cores a kept :term:`isolated <Core isolation>`.

The core allocation scheme may be static (e.g., a :term:`PNF` with
purpose-built hardware and a fixed number of CPU cores), or dynamic in
regards to the hardware platform properties (core count, CPU cache and
NUMA hierarchy, etc).

The CPU resource manager must be equipped with a basic understanding
of the fast path application black box constraints, e.g.:

* The minimum number of dedicated CPU cores required for :term:`fast
  path lcores <Fast path lcore>`.
* The maximum number of dedicated CPU cores the fast path application
  is able to make use of.
* In case of a :term:`SMT` system, how should the :term:`hardware
  threads <Hardware threading>` be employed, e.g. one (or a
  combination) of the follow options:

    A. Allocate one hardware thread per :term:`physical core` to the
       fast path application, and leave the sibling hardware threads
       idle.
    B. Allocate one hardware thread per :term:`physical core` to the
       fast path application, and use the siblings for other non-fast
       path threads, or enable them in the control thread core mask.
    C. Allocate all siblings of a particular physical core to the fast
       path, and let it sort out how they are best used (e.g., by
       digging into the CPU topology via /proc, or just ignore SMT
       topology).

* The minimum and maximum number of shared CPU cores, for data plane
  control and other control threads.
* For a heterogenous system, if small or large CPU cores, or a
  combination thereof, are preferred.

Usually, one or more cores are reserved for :term:`data plane
control`, :term:`control plane`, or :term:`management plane` use, or
for the use by DPDK-internal threads, such as the interrupt thread.

Allocation would normally occur during DPDK application startup. See
the section on :ref:`Lcore affinity` for more information. The number
of EAL threads created puts an upper limit on how many CPU cores may
be utilized by :term:`EAL threads <EAL thread>`. The limit is fixed
across a DPDK process' lifetime.

Which :term:`logical cores <Logical core>` should be used may be
specified by means of the ``-l`` or ``-c`` :term:`EAL parameters`, at
program startup.

Which CPU cores are to be used for the DPDK :term:`control threads
<Control thread>` can also be controlled, by setting the appropriate
initial DPDK process :term:`affinity mask <Processor
affinity>`. Consider an application ``eavesdropper`` run on a system
with 24 cores, numbered 0-23 (inclusive)::

  taskset -c 1-23 eavesdropper -l 3-23

The ``taskset`` utility runs the specified program, with the affinity
setting provided. In this invocation, taskset will set initial
eavesdropper ``main()`` thread affinity to include cores numbered 1
to 23. ``-l 3-23`` will instruct :term:`EAL`, via ``rte_eal_init()``,
to use the logical cores numbered 3 to 23; one EAL threads per CPU
core. Any DPDK control threads created will receive an affinity mask
consisting of logical cores 2 and 3.

Internal Allocation
^^^^^^^^^^^^^^^^^^^

A task related to, but dinstinct from, core allocation is to decide
in what role each of the available lcores will serve *within* the fast
path application. This in turn is much related to the application
architecture, for example how :term:`work scheduling <Work scheduler>`
is implemented.  The organization of the packet processing pipeline,
and the division of concearns between lcores, and the implementation
of :term:`concurrency` is an decidely application-internal questions,
and a topic left to future chapters.

Service Lcore Allocation
^^^^^^^^^^^^^^^^^^^^^^^^

For applications that use the DPDK :term:`service cores framework`,
there is a need to configure a number of the DPDK-managed lcores
to be used for running services.

Which lcores are to be used as service cores may specified using
:term:`EAL parameters`. Configuring service cores by command-line
options make sense for driver-level service core usage, in particular
if said PMD is also instantiated using EAL parameters.

When service cores are used for application-level (i.e.,
non-DPDK-platform level) services, the conversion of :term:`worker
lcores <Worker lcore>` to :term:`service lcores <Service lcore>` is
likely best managed by the application itself, being an
process-internal implementation detail.

Thread Type Summary
-------------------

.. list-table:: Data Plane Thread Types
   :widths: 30 25 25 50 25
   :header-rows: 1

   * - Type
     - Creation
     - Has Lcore Id?
     - Processor Affinity
     - Usually Risks Preemption
   * - | :term:`EAL thread`
       | :term:`Main lcore`
       | :term:`Worker lcore`
       | :term:`Service lcore`
       | :term:`Fast path lcore`
     - EAL
     - Yes
     - Almost always :term:`pinned <Processor affinity>` to a dedicated core.
     - No
   * - :term:`Registered non-EAL thread`
     - Application e.g. via ``pthread_create()`` + ``rte_thread_register()``.
     - Yes
     - Unspecified
     - Yes
   * - :term:`Unregistered non-EAL thread`
     - Application e.g. via ``pthread_create()``.
     - No
     - Unspecified
     - Yes
   * - | :term:`Control Thread`
       | :term:`Interrupt Thread`
     - EAL or application via ``rte_ctrl_thread_create()``.
     - No
     - Original pre-``rte_eal_init()`` main thread affinity, with the
       lcore CPU cores removed.
     - Yes

.. _Service Cores:

Service Cores
-------------

.. _A Terminology Side Note:

A Terminology Side Note
-----------------------

The misleading use of *lcore* and its derivate terms in DPDK has a
historical basis. The terminology dates back to pre-2.0.0 version of
the software. In early DPDK incarnations the lcore was a hardware
:term:`logical core` drafted into service as a DPDK lcore, and was
represented by and in various data in the core DPDK framework (the
:term:`EAL`), libraries, and :term:`PMDs <PMD>`.

When the affinity requirement was relaxed in DPDK 2.0.0, the lcore and
related names were kept, even though the DPDK lcore was no longer
necessarily tied to a logical core. The *lcore* was now just another
word for an *EAL thread*, according to the documentation.

The situations deteriorated further when the rte_thread_register()
function was introduced into the DPDK 20.11 public API. This function
allowed its user to create what the documentation refers to as
*registered non-EAL threads*, that had a *lcore id*, and was counted
by ``rte_lcore_count()``, both suggesting it was indeed an
lcore. However, these threads are excluded from ``RTE_LCORE_FOREACH``,
suggesting they were not lcores.

Although the current state of affairs resulted a fair amount headache
and verbiage for dataplane book authors, it doesn't cause much trouble
in data plane software developers' everyday DPDK
discussions. [#headache]_ There are two reasons for this:

1. The defacto one-to-one relationship between lcore and CPU core is
   even in contemporary applications *almost* always true. This
   subject is gets an in-depth treatment in the :ref:`Lcore Affinity`
   section.
2. :term:`Registered non-EAL thread <Registered non-EAL thread>` are
   releatively scarce and does not serve a central roll, especially
   for the DPDK platform itself. Thus, their complicated, incoherent
   nature, where they are sometimes an EAL thread (lcore) and
   sometimes not, is rarely exposed.

.. _Real Time Scheduling Policies:

Real Time Scheduling Policies
-----------------------------

.. rubric:: Footnotes

.. [#alternatives]
   An application running on *bare metal* (in the original sense of
   the word, i.e. a system bare of anything resembling an operating
   system), with supervisor capabilities, and carrying its own
   special-purpose kernel, would have other options.

.. [#cppthreadoptions]
   Nowadays, coroutines and fibers are options to using POSIX threads
   for concurrency in C++. This has long been true for C, where many
   of the original threading libraries had threads that was actually
   fibers (i.e, the thread switches were performed in user space). The
   term *green threads* were used as the time. In C, the practice
   seems to have fallen out of favor.

.. [#unix]
   UNIX here is not only meant to refer to the set of
   now-largely-extinct operating system that are trademarked UNIX, but
   also those that are API compatible with the POSIX APIs, such as
   FreeBSD and Linux.

.. [#mainthread]
   The original thread that called the program's main() function is
   employed as a EAL thread for the main lcore, and thus no thread
   need to be spawned for this lcore.

.. [#logicalcoreperformance]
   The performance characterstics may vary wildly.

.. [#noterm]
   Presumably the reason is that the authors assumed that EAL threads
   are always employed as fast path lcores, and thus there's no need
   for a separate term.

.. [#headache]
   The author suspects the plentphora of terms required to talk about
   DPDK threads in a reasonbly precise manner might result in some
   headache also for the readers of this text.

.. [#ladawait]
   The quality of experience of such a data plane would be on par with
   the automotive industry of the Soviet Union, where the customer was
   asked to wait 10 years for the delivery of their new car.

.. [#1ton]
   Technically, such applications would likely allow a 1:N
   relationship between EAL thread and CPU core, as long as the N
   cores used are dedicated to that EAL thread. The author has trouble
   seeing such a deployment as anything but a waste of (N-1) perfectly
   serviceable CPU cores.
