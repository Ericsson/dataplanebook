..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

Glossary
********

.. glossary::

   ACL
      An Access Control List (ACL) regulates access to a resource. In
      data plane applications, ACLs usually have a match part,
      where the user specifies which packets this rule should apply to,
      and an action part, which specifies what should be done to
      matching packet (e.g., drop or accept).

   Amdahl's law
      Amdahl's law gives the theoretical performance gains
      (in terms of reduced :term:`processing latency`) when multiple
      processors are used to complete a partly parallel task.

   ASIC
      An application-specific integrated circuit (ASIC) is a chip
      built to implementation a particular function, or set of related
      functions.

   ATM
      Asynchronous Transfer Mode (ATM) is a type of fixed (wired)
      telecommunications network, that has largely fallen into disuse.
      As opposed to IP, ATM is connection-oriented on the :term:`data
      link layer`.

   ARP
      The Address Resolution Protocol (ARP) is used in conjunction
      with IPv4 for discovering the link layer (e.g., Ethernet MAC
      address) for a particular IPv4 address.

   BGP
      The Border Gateway Protocol (BGP) is a protocol used by Internet
      routers to exchange routing and reachability information.

   Bottom half
      See :term:`top half`.

   Data plane platform
      The part of the data plane applications that provides hardware
      abstractions and the associated hardware drivers, and other
      operating system-like services like work scheduling, memory
      management, and timers. :ref:`DPDK` is the reference data
      plane platform of this book.

   Data race
      A data race occurs when two or more threads access shared
      memory, without proper synchronization. At least one thread must
      be a reader, and at least one a writer. A data race may cause
      nondeterministic program behavior, with different results
      produced between different runs of the same program, due to
      random or pseudo random conditions such as the interleaving of
      the program's threads. A program containing a data race may also
      produce different results depending on which :term:`ISA`, CPU,
      compiler, or compiler flags are used.

   DVFS
      Dynamic voltage and frequency scaling (DVFS), sometimes
      shortened to just *frequency scaling*, is a CPU feature in where
      the operating frequency and the voltage for a particular
      :term:`physical CPU core <physical core>`, or cluster of cores,
      may be reduced, to improve energy efficiency (e.g., the amount
      energy required to finish some task). DVFS may be available in
      the core itself, for a cluster of core, or for non-core hardware
      blocks such as the interconnect, or accelerators.

      CPU core frequencies is either controlled by the automoously by
      the hardware the by kernel, or by a user application, using
      kernel runtime configuration to do so.

   eBPF
      Extended Berkeley Packet Filter (eBPF) is a low-level
      programming language. The original version of BPF was, just as
      the name suggests, used for network packet filtering. The
      current Linux kernel support more eBPF-related uses cases,
      allowing eBPF program to be attached to other events than an
      arriving network packet, such as a system call.

      eBPF has a :term:`ISA`-independent byte code format. A developer
      (or a tool) has the option of authoring the eBPF program in this
      byte code format, or a subset of C. In the latter case, the LLVM
      clang compiler (and its eBPF backend) may be used to compile the
      source code into a eBPF byte code.

      The Linux kernel has a virtual machine (including a
      just-in-time compiler) for eBPF byte code.

      DPDK also comes with an eBPF virtual machine, similar to that
      found in the Linux kernel.

      A characterizing property of eBPF programs is that there is always
      an upper bound to their execution time.

   :ref:`CNF <Network Function>`
      A container network function (CNF) is a :term:`network function`
      hosted in a container.

   Event fd
      An event :term:`file descriptor` is a reference to a
      kernel-level event fd object, designed to be used by user
      application for thread synchronization. An event fd object has
      sempahor-like semantics, but unlike POSIX semaphores, event fds
      are proper fds and thus may be used in e.g. ``select()``.

   Cache line
      A cache line is the smallest unit managed by the cache hierarchy
      of current-day :term:`SMP` systems. A cache line usally holds 64
      bytes of data, and in some relatively rare cases 128 bytes.

   Communications Processor
      An older name for a :term:`DPU`.

   :ref:`Control plane <Control Plane>`
      The part of the network that negotiates, computes or otherwise handles
      higher-level policies, such as how routing is set up, and makes sure
      they take affect in the Data Plane.

   :ref:`Control thread <Control Threads>`
      A control thread is a thread running as a part fast path process,
      responsible for serving process-external interfaces, translating
      requests into calls into the fast path's internal APIs. Unlike
      their lcore worker thread counterparts, the control threads usually
      don't run on dedicated CPU cores.

      In a DPDK application, the term has a slightly different
      meaning, both more specific and with a wider scope.  A DPDK
      control thread is a thread created as the result of a
      ``rte_ctrl_thread_create()``, and begins its life as a
      :term:`unregistered non-EAL thread` operating system thread,
      with the :term:`processor affinity` set in such a way, all the
      CPU cores used for EAL threads are removed. Such a control
      thread may take the role described above, or it may be used in
      some other, completely different, manner.

   Concurrency
     Two or more tasks are considered to be execution concurrently if
     their processing seems to occur roughly across the same time
     span, giving the course-grained impression of
     :term:`parallelism`.

     If the tasks are run by software threads running on a multi-core
     CPU, their execution may indeed be parallel. If more ready-to-run
     threads are available than there are CPU cores available,
     multitasking, with the assistance of the kernel's process
     scheduler, may be employed to maintain concurrency (without full
     parallelism).

   Context switch
     A process context switch, or context switch for short, is the act
     of replacing a thread (belong to a particular process or the
     kernel) executing on a particular CPU core with another thread
     (of the same process or another process), on that core.

     In Linux terms is replacing the executing *task* with another
     task.

     The term context switch is occasionally used for the processing
     required when making a system call, which is done for a different
     purpose, but where the steps required are much the same (e.g.,
     switching stack).

   Core isolation
      An isolated core is a CPU core managed by the operating system
      kernel, but for which steps are taken to dedicated its use
      solely to a particular application, to the extent possible.

      For an isolated core, the kernel is configured to disallow
      scheduling of all other user space threads and all unbound
      kernel threads and interrupts.

      An application thread running on a isolated core will be able to
      run essentially uninterrupted, even without the use of real-time
      scheduling policies. However, short interrupts from kernel house
      keeping threads bound to that core may still occur. There may
      also be other sources of discontinuity is the application
      thread's execution, for example in the form of hardware-level
      delays related to core frequency changes or non-maskable
      interrupt (NMI) handling.

      The periodical timer interrupt may be disabled by using a
      "tickless" kernel.

      Completely interruption free operation is in general not
      possible in the operating systems employed for the data plane
      applications of this book, but are in general also not strictly
      required.

   Core mask
      A bitmask which selects a number of :term:`CPU cores <CPU core>`
      from the available set of cores.

      In the context of DPDK, it's used to specify which of the
      kernel-level :term:`logical cores <Logical core>` should be used
      by a DPDK application process as DPDK :term:`lcores <Lcore>`.

      The first bit in a DPDK core mask corresponds to the CPU core
      the kernel refers to as id 0, the second bit as id 1, etc.

   CPU
     The Central Processing Unit (CPU) is the main processor in a
     computer. A CPU usually has a fairly general-purpose instruction
     set, and may or may not be the processor in the system the wields
     the most computational horse power.

     The introduction of CPUs chips with multiple :term:`cores <CPU
     core>` left the term fuzzy. CPU can be used to mean the chip as a
     whole, including all CPU cores, and occasionally the
     interconnect, CPU caches, and memory controllers as well. It may
     also be used in the more software-centric sense of :term:`CPU
     core`, or the technically more accurate, :term:`logical core`.

     This book sticks to what seems like the currently more frequently
     used meaning; the whole chip, or indeed all chips working in
     concert to implement a single :term:`SMP` (e.g., a multi-socket
     server).

     The Linux kernel use the term in the :term:`logical core`
     meaning. Early multi processor system had multiple distinct CPU
     chips, so when the multi processor support was developed, there
     existed no ambiguity.

   CPU core
     A :term:`CPU` core, or just *core*, is a piece of electronic
     circuitry that executes instructions, that comprises a computer
     program.

     A non-:term:`SMT` core executes a single program at a time.  A
     :term:`SMT` core processes two or more instruction streams in
     parallel.

     For texts taking a software perspective, the term is often used
     to mean :term:`logical core`. The reader should beware that the
     book may resort to this somewhat imprecise usage form.

   CN
      The Core Network (CN) is the network that sits between the :term:`RAN`
      and the Internet in a mobile telecommunications system, such as LTE.

   :ref:`Data plane <Data Plane>`
      The part of the network that handles that actual user data. Also known
      as the User Plane, or the Forwarding Plane.

   :ref:`Data plane control <Data Plane Control>`
      The part of the data plane application that terminates
      interfaces external to the network function (e.g., for
      configuration or observability).

   Critical section
      Critical section (also known as *critical region*) is a section
      of the program which cannot be executed by more than one thread
      in parallel. This may be achieved by means of a lock.

   Data link layer
      The data link layer is another name for :term:`layer 2` in the
      :term:`OSI model`. A related term is the :term:`link layer` of
      the Internet :term:`protocol suite <Network protocol suite>`

   Domain logic
      Domain logic, also known as business logic, is the part of a
      program that directly corresponds to it's core function, from a
      black box perspective. For example, the source code fragments in
      an IP stack responsible to decide when and how an ICMP Time
      Exceed packet is generated is domain logic. Code in the same
      stack to implement a linked list or code to manage the
      distribution of processing tasks across CPU cores are not.

   DPU
      A Data Processing Unit (DPU) is processor designed for data
      plane applications. Largely a marketing term, how a DPU is
      implemented, as opposed to what role it serves, is somewhat
      vague. A seemingly popular design is to build a DPU around a
      complex of general-purpose :term:`SMP` CPU cores, augmented by
      networking-specific accelerators and high performance network
      I/O interfaces.

      The general-purpose cores and the associated memory hierarchy
      may be designed and dimensioned to be involved in fast path
      processing, or only be used for slow path and control plane type
      tasks. In the latter case, a :term:`NPU` type block will be
      required as well, to facilitate a software-programmable fast
      path.

      Older generation processors with the built for the same purpose,
      with the same basic architecture is referred to as communication
      processors.

   EAL
      The DPDK Environment Abstraction Layer (EAL) is the core of the
      DPDK framework. As the name suggests, it is, to some extent,
      used to hide the underlying operating system APIs. DPDK has a
      relaxed attitude toward OS abstraction, and direct POSIX calls
      are common in non-EAL code.

      The EAL also hosts a variety of generic, low-level services,
      such as modules for heap memory management, pseudo random number
      generators, and a wide variety of synchronization primitives
      (e.g., spinlocks). EAL is also the home of the :term:`service
      cores framework`.

   EAL parameters
      At program invocation, the :term:`EAL` of a DPDK application
      may be configured by setting `EAL parameters
      <https://doc.dpdk.org/guides/linux_gsg/linux_eal_parameters.html>`_
      . Such parameters are supplied by means of command-line options,
      and may be used to control memory usage, configure :ref:`core
      allocation <Core Allocation>`, instantiate virtual devices,
      enable logging and more.

   EAL thread
      An EAL thread is an operating system thread created and managed
      by the DPDK :term:`EAL`, with some associated DPDK-level data
      structures. An alternative name is :term:`lcore`.

      An EAL thread is identified by a :term:`lcore id`.

      Many DPDK APIs intended to be used in the fast path may only be
      called by a EAL thread (or a :term:`registered non-EAL thread`),
      often with the additional requirement that the thread needs to be
      :term:`non-preemptable <Non-preemptable thread>`.

   Exception traffic
      Exception traffic consists of a type of packets, which during
      normal network conditions are infrequent, that need more complex
      processing. For flow-based forwarding engines, this could be the
      first packet in a previously unseen flow, and as such requires
      checking against security policies and the installation of a new
      entry in the fast path's forwarding database. It may also be an
      ARP request, or a fragmented IP packet, for a limited-feature
      fast path IP stack.

   False sharing
      False sharing occurs when multiple CPU cores accesses two or
      more pieces of logically disjoint data resides on the same CPU
      :term:`cache line`. For false sharing to have any detrimental
      effects, at least one core need to write to the cache line. The
      effect is a performance degradation, the size of which depends
      on the frequency of access. False sharing does not affect the
      correctness of the program, but may dramatically increase the
      time and energy used to complete its task.

   :ref:`Fast path <Fast Path>`
      The data plane fast path is part of the data plane application that
      handles the bulk of the packets.

   Fast path lcore
      For the purpose of this book, a :term:`lcore` used for
      throughput and latency- sensitive tasks, usually in the form of
      fast path packet processing, in a DPDK-based data plane fast
      path application, is referred to as a fast path lcore.

      A fast path lcore may be any role; the :term:`main lcore`,
      a :term:`worker lcore`, or a :term:`service lcore`.
 
   FIB
      A Forward Information Base (FIB) holds information on where to
      forward a packet.

   File descriptor
      A file descriptor (often abbreviated fd) is a handle, in the
      form of a non-negative integer, referencing a kernel-level
      object in UNIX or UNIX-like systems. Following UNIX' "everything
      is a file" philosophy, the kernel object may, besides being a
      file proper, be a network socket, a timer, a fd for receiving
      UNIX signals, and several other types of I/O devices and other
      event sources.

   Floating thread
      A floating thread is an operating system thread which
      :term:`processor affinity` mask makes it eligable to be
      scheduled on more than one core.

   Flow cache
      A flow cache is a data structure which is logically an overlay
      on top of the complete :term:`FIB`. Systems that employ a
      flow cache avoid having to perform a potentially costly FIB lookup
      (among other processing, such as :term:`ACL` lookup operations) for
      every packet in a flow.

   Forwarding plane
      A synonym to data plane, often used for in the context of switches
      and IP router implementations.

   Frame
      A frame is a :term:`data link layer` :term:`PDU` (e.g., an
      Ethernet frame).

   Full core
      A full core is colloquial term for either a :term:`SMT`
      :term:`CPU core` where all but one of the hardware threads are
      left unused (or disabled), or a non-SMT core.

   User thread
      A user thread is a thread which is managed not by the kernel,
      but by some userspace entity, such a library, language virtual
      machine runtime or the application itself.

   Hardware threading
      Hardware threading is a design technique where a CPU core is
      divided into two or more virtual CPU cores, called *hardware
      threads*. From a software point of view, each such hardware
      thread looks just like a "real" CPU core, with its own set of
      registers, a stack, etc, and adhering to the appropriate
      :term:`ISA`. However, on the level of the physical
      implementation, each hardware thread share, to a varying degree,
      underlying CPU core resources (e.g., core-private caches, shadow
      registers, instruction decoders, arithmetic logic units, etc.)
      with one or more hardware threads on the same core. Hardware
      threads hosted by the same underlying :term:`physical CPU core
      <Physical core>` are usually referred to as siblings.

      The number of hardware threads is fixed, and unlikely their
      software counter parts, hardware threads do not migrate across
      physical cores.

      There are two types of hardware threading, temporal
      multithreading and :term:`simultaneous multithreading <SMT>`. In
      simultaneous multithreading, two instructions streams may make
      use of the same CPU pipeline stage at the same time (i.e,
      cycle). In CPU implementing temporal multithreading, at a
      particular time, only a single stream use a particular pipeline
      stage.

      With hardware threading, the physical core has two or more
      independent instructions stream to execute, allowing for greater
      level of utilization its resources. For example, if one stream
      of instruction depends on a high-latency memory load operation
      to finish before further progress can be made, the other
      hardware threads can make full use of the core's resource
      meanwhile.

      Generally, when siblings threads are actively being used, the
      serial performance of the core drops. Thus, on SMT core where
      all hardware threads are busy, the :term:`wall-clock latency`
      to finish a particular computation is likely higher compared
      to if only a single hardware thread was active, or if SMT
      was disabled altogether.

      SMT is the most common form, implemented in many 64-bit x86 CPUs
      from Intel and AMD. Those SMT implementation generally improve
      the aggregate performance of the core with roughly 25%, although
      the actual effect depends much on the application. In certain
      extreme cases, SMT may even degrade aggregate throughput (e.g.,
      due to the increase of the total :term:`working set size` of
      the threads' instruction streams).

   Heterogeneous multiprocessors
      A heterogeneous multiprocessor is a :term:`SMP` multicore CPU,
      with a heterogeneous CPU topology in the sense that some cores
      are faster than others. The faster cores are usually physically
      bigger and equipped with larger caches, and may also operate on
      a higher clock frequency.

   High touch application
      A data plane fast path application that on average spends relatively
      many CPU clock cycles and other hardware resources for every packet.

   Huge pages
      The virtual address space is divided into pages, usually 4 kB
      in size. The hardware keeps a cache of translation between
      virtual and physical in a Translation Look-aside Buffer (TLB).
      For applications accessing a large amount of memory (i.e., with
      a large working set size), the TLB cache may be missed, causing
      expensive traps to the kernel. Increasing the page size for
      part of the virtual memory is a way to avoid this issue. Such
      pages are often very much large (e.g., 2 MB or 1 GB), and thus
      are often referred to as "huge pages".

   Interrupt thread
      A DPDK control thread used to process hardware interrupt
      notifications from the kernel.

   IPC
      Instruction per cycle (IPC) is a measure on how many
      instructions a particular CPU core retires, on the average, per
      clock cycle.

      IPC is often used to compare the performance of different
      implementations of a particular :term:`ISA`. Various changes to
      a CPU core or the CPU cache hierarchy or memory interfaces may
      lead to a higher IPC, which in turns leads to higher performance
      at the same clock frequency.

      IPC may also be used an indication of program
      efficiency. According to this rule of a thumb, a program
      resulting in high IPC is considered highly optimized, and a
      program with comparatively low IPC the opposite. However, this
      rule does not take time complex of the program's algorithms into
      play. It may well be that the high-IPC program implements a very
      naive algorithm, which results in the CPU having to execute a
      very large number instructions, but may do so at a high IPC, as
      opposed to a variant where the same task is achieved with a
      lower time complexity algorithm, which requires fewer
      instructions to be run, but potentially at a lower IPC. An
      example such a situation is one program using linear search to
      find an item in an array, with good IPC and an easy-to-handle
      situation for the CPU hardware prefetcher, and one using a
      binary tree, which results in both more cache misses and branch
      mispredictions, causing the IPC to be lower. Still, the latter
      may on average be much quicker to find the needle in the
      haystack.

      Another situation that may occur, are two programs implementing
      the same basic algorithm, but one using simple non-vector
      instructions and the other using more complex and costly (on a
      per-instrution basis) :term:`SIMD` instructions. The latter will
      have likely have better performance, but may well have lower
      IPC than its non-SIMD counterpart.

      IPC is also commonly used abbrevition of inter-process
      communcation.

   ISA
      An Instruction Set Architecture (ISA) specifies the interface
      between software and the CPU hardware. The ISA defines things like
      the available machine language instructions (and how they
      are encoded), registers, data types and memory models.

   Item of work
      A task given to a thread. In the data plane, most items of work
      are directly related to packets, and the work descriptor
      contains a packet buffer pointer, and some associated meta data
      (e.g., what kind of processing should be done).

      An item of work may also be a timer timeout, a completion
      notification from an accelerator (e.g., a crypto block), or a
      request from :term:`data plane control` to update a table, or
      retrieve some information about the state of the fast path.

      In DPDK Eventdev, the item of work is referred to as an *event*.

   Jitter
      Jitter is a measure of latency (i.e., delay) variation.

   Kernel thread
      A kernel thread is an :term:`operating system thread` which runs
      exclusively in kernel space.

      Kernel threads perform background tasks in the Linux kernel. A
      kernel thread shares most characteristics with its :term:`user
      space cousin <User space thread>`, including the fact that it
      may be preempted and replace by another thread, of the kernel or
      user space variety.

   Layer 1
      The physical layer is the first layer in the OSI model.

   Layer 2
      The data link layer is the second layer in OSI model, and handles
      data transmission between different nodes on the same physical
      network segment. Ethernet is an example of a layer 2 data link
      layer protocol.

   Layer 3
      The network layer is the third layer in the OSI model, and
      handles communication between different hosts on the same or
      different :term:`layer 2` network segments. IPv4 and IPv6 and
      examples of network protocols.

   Lcore
      A seemingly DPDK-specific abbreviation of :term:`logical core`.

      The DPDK term is related to the generic hardware-level term,
      but is a software concept, and occasionally not tied to
      a particular logical core.

      When this book uses the term in its abbreviated form, it is
      referring to the DPDK meaning of the word.

   Lcore id
      A DPDK framework level identifier for an :term:`EAL thread` or a
      :term:`Registered non-EAL thread`. The lcore id takes the form
      of a non-negative number in the range from 0 up to (but not
      including) ``RTE_MAX_LCORE``.

      In most DPDK application deployments, an EAL thread's lcore id
      corresponds to a particular CPU core id. Unless otherwise
      specified (e.g, by :term:`EAL Parameters`), the lcore id and the
      kernel-level CPU core id has the same value for EAL threads.

      Registered non-EAL threads are given higher-numbered, previously
      unused, lcore ids.

   Link layer
      The link layer is the lowest layer in the Internet
      :term:`protocol suite <Network protocol suite>`. It consists of
      functionality similar to that of the :term:`OSI model <OSI
      model>` allocates to its :term:`layer 1` and :term:`layer 2`.

      Ethernet is an example of a link layer.

   Logical core
      A logical core is an entity, usually a piece of hardware, that
      behaves like a :term:`CPU core` from the point of view of a
      computer program. A logical core may be a non-:term:`SMT`
      physical core (often referred to as a :term:`full core`), a
      :term:`hardware thread <Hardware threading>`, or a core emulated
      in software (presumably on top of some physical hardware).

      When the term is used in a DPDK context, usually abbreviated to
      :term:`lcore`, it refers to an :term:`EAL thread`. 

   Load
      A load machine instruction reads a chunk of data (usually 8-512
      bits) from memory and puts it into a CPU register.

   Lock contention
      Lock contention occurs when a thread attempt to acquire an
      already-held lock. A highly contended lock is a lock where a
      lock operation (e.g., ``rte_spinlock_lock()``) often results in
      contention.

   Low touch application
      A data plane fast path application that on average spends relatively
      few CPU clock cycles and other hardware resources for every packet.

   LTO
      Link-time Optimization (LTO) is a compiler mode of operation,
      where optimizations are deferred to the link stage, allowing
      optimization to be done across program's or shared library's
      different compilation units. The inlining of a function residing
      in a different .c file than the caller is possible, for
      example. LTO increases build times to such a large degree that
      it is often impractical to use.

   Main lcore
      The DPDK :term:`lcore` responsible for DPDK framework
      initialization is referred to as the main lcore. The thread that
      runs the application's main() function will be used as the main
      lcore's :term:`EAL thread`. Post-initialization the main lcore
      have no special tasks or status compared with other lcore, as
      far the DPDK platform is concerned, and may for example be
      employed as a :term:`fast path lcore`.

      By default, main role is assigned to the lowest-numbered CPU core
      in the :term:`core mask`, but may be configured to be any
      DPDK application lcore.

   MIB
      A Management Information Base (MIB) is a :term:`SNMP` data model.
      The term is sometimes also used to refer to an instance of a
      particular model. IEFT has defined a number of MIBs (e.g., for
      TCP and IP).

   :ref:`Management plane <Management Plane>`
      The part of the network that handles configuration and
      monitoring.

   MT safe
      Multi-thread (MT) safe functions, also known as thread-safe
      functions, may safely be called by multiple threads
      simultaneously.

   Multiprocessing
      In the context of operating systems, multiprocessing refers to
      the ability to run multiple processes on multiple CPU cores, in
      term:`parallell`.

      In the context of CPU architecture, multiprocessing is what
      a multiprocessor (e.g., a :term:`SMP` system) does.

   Multitasking
      Multitasking is the ability of an operating system to
      :term:`concurrently <Concurrency>` executing multiple task. The
      kernel's process scheduler will frequently (by human standards)
      switch from one task to the other, creating the impression of
      :term:`parallel <Parallelism>`, even in situations where there are
      more runnable tasks than there are CPU cores in the system.

   Multithreading
      Multithreading is the use of multiple :term:`threads <Operating
      system thread>` in the same operating system process.

      As oppposed to threads in different processes, all threads in
      the same process shared the same memory. Even data that is
      generally considered thread-private, such as the stack and data
      in :term:`thread-local storage <TLS>`, is accessible to other
      threads as well.

   Mythical Man-Month
      In the book titled *The Mythical Man-Month: Essays on Software
      Engineering*, Fredrick Brooks of IBM debunks the myth that a
      software project can be estimated in man-months. In particular,
      he observes that the communication overhead grows in non-linear
      fashion as people are added to the project.

   NAT
      Network Address Translation (NAT) is a method of rewriting the
      IP packet header to translate to change the source and/or
      destination host and/or port, often for the purpose of having
      multiple IP hosts to between host's and its single IP address.

   ND
      Neighbor Discovery (ND) is a protocol operating at the link
      layer. It may be employed in the same role has :term:`ARP` has
      for IPv4 (i.e., resolving an IP address into a link-layer
      address). ND is also used for router discovery and router
      redirection.

   NETCONF
      The Network Configuration Protocol (NETCONF) is an XML-based
      network configuration management protocol developed by the IEFT.

   Network application
      A network application receives input from, and produces output
      to, a computer network. Examples are a web browser, a DHCP
      server, or a micro service serving as a part of a distributed
      web application.

   :ref:`Network function <Network Function>`
      For the purpose of this book, the data plane application and its
      immediate surroundings, which work in concert to provide a data
      plane function to interface with entities in the control plane
      and other instances of data plane functions.

   Network protocol suite
      A set of related communication protocols, usually arranged in
      layered architecture, used in a computer network.

   Network layer
      The network layer is another name for :term:`layer 3` in the
      :term:`OSI model`.

   Network stack
      A network stack, also known as a protocol stack, is an
      implementation, usually in software, of a family or
      :term:`suite<network protocol suite>` of network protocols.

   NIC
      A network interface controller (NIC) is a piece of hardware that
      connects a computer to a computer network. On one side of the
      NIC is a bus of some sort (e.g., PCIe), and on the other is a
      network interface, either the physical layer of an local
      network, or an on-chip link to some other hardware module (e.g.,
      a switch).

      A NIC is also known as a network interface card (even in cases
      where it's not implemented as a discrete card) or a network
      adapter.

   Noisy neighbour
      An application is considered a noisy neighbour in case it causes
      performance degradation for other applications, running on a
      different set of :term:`logical cores <Logical core>`, because
      of its extensive use of shared hardware resources, such as
      caches, memory, or I/O devices.

      Some CPUs support hardware-level mechanism, such a cache
      partitioning, to mitigate this issue.

   Non-blocking algorithm
      A non-blocking algorithm is an algorithm where the failure or
      suspension of one thread cannot result in the failure or
      suspension of another thread.

   Non-EAL thread
      A thread in a DPDK application process which is not a :term:`EAL
      thread`.

      There are two types of EAL threads; :term:`Registered non-EAL
      threads <Registered non-EAL thread>` and :term:`Unregistered
      non-EAL threads <Unregistered non-EAL thread>`.

   Non-preemptable thread
      A non-preemptable thread is a thread that never need to suffer
      an interruption of its execution due to an involuntary context
      switch, or the execution of a interrupt service routine.

      In the kernel, the execution of a critical section can usually
      be guaranteed to be performed without preemption (e.g., by
      disabling interrupts). The same is not true for user space
      threads in general-purpose operating systems - even for threads
      with a real-time scheduling policy.

      This book will use a more relaxed definition of this term,
      which aligns with the DPDK requirements in this area.

      * A non-preemptable thread may never be preempted and replaced
	with another non-preemptable thread within the same process.
      * A non-preemptable thread may be preempted, or its execution may
	otherwise delayed, but only for a short period of time.

      With this definition, user space threads may, assuming the
      appropriate system configuration, achieve a non-preemptable
      status.

      What qualify as a "short period of time" depends on application-
      level throughput, latency and latency jitter requirements.

   NPU
      A Network Processing Unit (NPU) (also known as network
      processor) is an integrated circuit designed for data plane fast
      path processing. A NPU is software programmable, but it's
      programming model usually differs in significant ways from a
      SMP processor. Programs of legacy NPUs were often limited in a manner
      similar to P4 and :term:`eBPF`, but the languages were proprietary
      or semi-proprietary (e.g., C-based but not full ANSI C), as were
      the tool chains.

      The original NPUs product lines, and the NPU term itself, has
      largely fallen out of use. However, in recent years there has
      been a resurgence of NU'S type designs in the form of highly
      programmable and flexible switch pipelines, either in switches
      circuits, or as a part of a :term:`DPU`.

   NUMA
      In a system which non-uniform memory access (NUMA), the access
      time experienced for a particular CPU core to memory varies
      if the memory is local or remote to that CPU.

      In practice, NUMA refers to DRAM access times. With the advent
      of CPU caches, *all* systems are NUMA in the sense that memory
      access varies with how far into the memory hiarcharchy the core
      needs to reach to retrieve the relevant :term:`cache line`.  A
      SMP system may well have some asymmetry when it comes to DRAM
      memory access characteristics, but it's only considered NUMA
      only when the difference in latency and bandwidth is significant
      enough to cause a significant performance degradation for
      applications with memory allocated in the "wrong" memory.

      In many early NUMA system the :term:`NUMA node` and the CPU
      socket boundaries coincides.

      In addition to the memory latency, the bandwidth available to a
      core may also significantly differ between local and remote
      memory. Normally, in a NUMA system, the kernel will attempt to
      allocate memory local to the same core the allocating thread is
      currently scheduled on.  The operating system may also attempt
      to migrate memory pages between NUMA nodes to reflect actual
      usage. Such migration introduces page faults and memory copies,
      and which in turn creates latency jitter.

   NUMA node
      A grouping of CPU cores, I/O buses and memory that are close
      to each other.

   Open vSwitch
      Open vSwitch (OVS) is a multi-layer Open Source software switch.
      OVS employs a :term:`flow cache` type approach to forwarding.

      The OVS combined :term:`control plane` and data plane
      :term:`slow path` process ``ovs-vswitchd`` may be paired with a
      number of different OVS fast path implementations, known as
      datapaths in OVS terminology. There is a Linux kernel OVS
      datapath and a DPDK-based datapath. There are also a number of
      hardware switches than can act as a OVS datapath.

   :ref:`Operating system thread <Threads>`
      A :term:`thread` which is scheduled and otherwise managed by the
      operating system kernel. There are two types of operating system
      threads; the :term:`user space thread` and the :term:`kernel
      thread`.

   OSI model
      Open Systems Interconnection (OSI) model is a conceptual model,
      describing an layered architecture, and the division of
      concearns among the layers.

   Packet
      A packet is a :term:`network layer` :term:`PDU` (e.g., an IPv4
      packet). Often, while technically incorrect, term is also for
      PDUs of other network protocol layers, such as the :term:`data
      link layer`.

   Parallelism
      The term parallel, as used in this book, is reserved for
      situations when two or more tasks are literally performed during
      the same, or at least overlapping, time period. The result of
      various time sharing schemes (e.g., multitasking or temporal
      :term:`hardware threading`), the term :term:`concurrency` is
      used instead.

      This books mostly concern itself with parallelism on the level
      of software threads, and their execution on CPU cores. In that
      case, parallel execution of two threads only occurs they are
      literally executed on different CPU cores (or :term:`hardware
      threads <Hardware threading>` on the same core), at the same
      time.

      A superscalar CPU core is also parallel in the sense that two
      or more instructions from the same instruction stream may be
      executed at the same time (e.g., using different core execution
      units, or at different stages at the CPU pipeline).

   PDU
      A protocol data unit (PDU) is a unit of information, transmitted
      between entities of some particular protocol layer.

      A PDU typically consists of a protocol layer-specific header and
      payload data, which itself may be part of a PDU for the next
      higher layer in the :term:`network protocol suite`.

      For example, the :term:`data link layer` PDU is the
      :term:`frame`.

   Peer preemptable EAL thread
      A peer preemptable EAL thread is an EAL thread which may be
      preempted by the kernel's process scheduler and be replaced with
      an EAL thread originating from the same DPDK process.

      A DPDK application is normally deployed in a manner which will
      prevent this scenario from ever occuring.

   Physical core
      The term physical core refers to the underlying electric
      circuitry that either implements a single :term:`logical core`,
      or, in the :term:`SMT` case, multiple such, in the form of
      hardware threads.

   Physical function
      See :term:`SR-IOV`.

   PMD
      In the early days of DPDK's history, the poll mode driver (PMD)
      was an Ethernet driver ported from FreeBSD to run in user
      space. Since interrupts couldn't easily and efficiently be
      routed to user space applications, the driver API was operated
      in a polling fashion.

      A present-day DPDK PMD is neither necessarily a driver of some
      hardware device, but also is not necessarily operated in a
      polled fashion.

      PMDs are also used for software-only implementations, which
      aren't hardware drivers at all, but just a concrete classes
      implementing a polymorphic interface.

      A common misconception is that PMDs refers only to DPDK's
      Ethernet drivers. That is *not* the case.

   PNF
      A physical network function (PNF) is a :term:`network function`
      in the form of a network appliance, usually the software running
      on purpose-built hardware.

   Preemption safety
      A operation is preemption safe in case the preemption of a
      thread's execution (e.g., a kernel-induced process context
      switch occurs) does not threaten the correctness of the program,
      or have very detrimental effects performance. In this book, the
      preemption unsafe constructs covered only cause performance
      degradations, although at time very serious such.

   Preemptable thread
      A preemptable thread is a thread which may suffer an involuntary
      context switch and other kind of kernel-induced interruptions.
      The opposite is a :term:`non-preemptable thread`.

   Processing latency
      For the purpose of this book, processing latency is the CPU time
      spent on a particular task (i.e., the number of CPU core
      cycles).  In case the processing is performed on multiple cores
      in parallel, the processing latency may be greater than the
      :term:`wall-clock latency`. In case a packet is buffered (e.g.,
      on the :term:`NIC`), and the data plane CPU cores are very busy,
      the processing latency may be only a small fraction of the total
      port-to-port wall-clock latency experience by that packet.

      In the context of IP routers, the term is used to denote all
      latency that occurs within the router (i.e., both CPU related
      latency and internal queuing latency). This is not how the term
      is used in this book.

   Priority inversion
      The term priority inversion is used to describe a scenario where
      a high-priority thread is prevented from executing, and instead
      is forced to have to wait for a lower-priority thread, usually
      because the low-priority thread holds resource lock.

   Processor affinity
     The process scheduler of a :term:`multitasking` operating system
     will usually, by default, be allowed to freely scheduled a
     particular thread to run on any of the available CPU cores.

     This degree of freedom left may be limited by configuring a
     thread's processor affinity, usually in the form of a bitmask. A
     thread is only eligible to run on a CPU cores which id number
     represented by a '1' in the thread's affinity mask.

     On Linux, processor affinity may be configured by the
     ``sched_setaffinity(2)`` system call, a the ``taskset(1)``
     command-line program, or via the ``/proc`` file system.

     The act of configuring processor affinity is also known as CPU
     pinning. The use of the term pinning is often restricted to
     situations where a thread limited to a single CPU core only
     (i.e., the thread is "pinned to a core").

   Program order
      Operations are said to be done in program order if the result of
      their execution is globally visible (e.g., to other CPU cores in
      the system) in the same order as the operations were specified
      in the program's source code.

   RAN
      The Radio Access Network (RAN) is the network that sits between
      the :term:`UE` and the :term:`CN` in a mobile telecommunications
      system.

   RCU
      Read-copy-update (RCU) is a synchronization technique which
      allows for efficient sharing of mostly-read data, accessed
      through a pointer.

      RCU exists in many variants. The most common in the data plane
      fast path is quiescent-state-based RCU (QSBR), an implementation
      of which is available in the DPDK RCU library.

   Registered non-EAL thread
      A registered non-:term:`EAL thread` is an operating system
      thread that was not created by the :term:`EAL`, but which
      registered itself with the EAL by calling
      ``rte_thread_register()`` in the `<rte_lcore.h> API
      <https://doc.dpdk.org/api/rte__lcore_8h.html>`_

      A registered EAL thread is allocated a :term:`lcore id`, with
      all the benefits that comes with such an id. It is not
      considered a DPDK :term:`lcore`, and thus for example
      are neither a worker nor the main lcore.

   RFS
      See :term:`RSS`.

   RSS
      Receive Side Scaling. A :term:`NIC` function which distributes
      packets to different NIC RX descriptor queues, usually based on
      the source and destination IP. If transport layer fields are
      taken into a account, the same function is sometimes called
      Receive Flow Scaling (RFS).

   Scheduling latency
      Sheduling latency is the :term:`time <Wall-clock latency>` from
      a thread could, in principle, be execute (i.e., it's *runnable*),
      to the time when it's actually running on a CPU core.

   Sequence counter
      A sequence counter is a low-overhead reader-writer synchronization
      mechanism.

   Service lcore
     A DPDK :term:`lcore` allocated to the :term:`Service cores
     framework`.

      *Core* in service core should be read as :term:`lcore`.

   Service cores framework
     `DPDK service cores
     <https://doc.dpdk.org/guides/prog_guide/service_cores.html>`_ is
     a DPDK concurrency and deferred work framework.

     The service core framework provide a means for software-backed
     DPDK :term:`PMDs <PMD>` to get work done. The framework may also
     be used by applications, to allow different and unrelated
     application modules and platform services to share the same set
     of :term:`lcores <Lcore>`.

     In the service cores framework, an application or the DPDK
     platform itself may register a service in the form of a worker
     function, and some meta data.

     The :term:`service lcores <Service lcore>` relies on cooperating
     multitasking, where the services configured on a particular
     service lcore is run in a round-robin fashion.

     Which lcores should be used as service cores, and which services
     should be running where is left to the application. Certain
     aspects may also be controlled by the DPDK command line options.

     The service cores framework may be used to break up the otherwise
     potentially rigid deployment architecture of a DPDK fast path
     application (i.e., how many cores the application requires, and
     what processing goes where).

     The service cores framework does *not* dynamically load balance
     services over available service lcores.

   SIMD
     A SIMD (single instruction, multiple data) instruction takes
     multiple data as input, performs an operation, usually the same,
     across all input, and (usually) produces multiple data as output.

     The SIMD instruction input and output registers are often called
     vectors, and SIMD instructions also goes under the name of vector
     instructions. This has spilled over on naming of compiler
     features, where generating SIMD instructions often is referred to
     as vectorization.

     An example of SIMD instruction sets are AVX extensions to the x86
     family :term:`ISAs <ISA>`, and AltiVec/VMX in PowerPC.

   Slow path
      The part of a data plane application that process exception traffic.

   SMP
      Symmetric multiprocessing (SMP) is a computer architecture
      style, where the processor has two or more cache-coherent cores
      with the same (or very similar) :term:`ISA`, sharing the same
      memory and I/O devices, and serving the same role (i.e., no CPU
      core is dedicated, on the level of the hardware, to handle some
      specific task). The original (but not this) definition required
      memory access times for a particular memory location should be
      the same across different CPU cores, which exclude the use of
      caches. General-purpose client and server x86 and ARM multi-core
      CPUs are all SMP CPU.

      Some definitions of require the CPU cores to be identical, which
      excludes :term:`heterogeneous multiprocessors`. The SMP
      definition of this book does not.

   SMT
      Simultaneous multithreading (SMT) is a :term:`hardware
      threading` technique implemented on the level of the CPU
      core. An SMT core work on two or more instruction streams in
      parallel.

   SNMP
      The Simple Network Management Protocol is a network management
      protocol for IP networks. Originally intended for configuration
      management, current-day use is primarily for network monitoring.

   Spinlock
      A type of lock where a thread failing to acquire a lock
      immediately retries, and keeps doing so ("spins"), until the
      lock operation is successful. Spinlocks are common in operating
      systems kernels, but unusual in user space applications, since
      they are not :term:`preemption safe <preemption safety>`.

   SR-IOV
      Single Root I/O virtualization (SR-IOV) is PCI Express (PCIe)
      virtualization standard.

      With SR-IOV, the PCIe device is split into a physical function
      (PF) and a number of virtual functions (VFs).

      The PF is primarily used for adminstrative tasks. A VF is used
      to access the actual PCIe device function (e.g., sending and
      receiving packets, in case of a PCIe :term:`NIC`).

      The PF is usually kept by the host, while the VFs are handed out
      to virtual machines or containers.

      The concept of a PFs and VFs are often used in situations, for
      non-PCIe devices, where "full" PCIe is not used (e.g, only
      PCI-like device enumeration), or not at all.

      The PF/VF division also reflects on the driver structure, where
      one driver is responsible for the PF, and another for the VFs.
      Often, the PF driver resides in the kernel, while the VF driver
      may be either in userspace (e.g., in DPDK), or in the kernel.

   SSH
      Secure Shell (SSH) is a protocol for remote shell access and
      command execution. It may also be used as a secure transport
      layer (e.g., for :term:`NETCONF`).

   System call
     A system call, or syscall for short, is a function call crossing
     the user-kernel space boundary.

   Store
      A store machine instruction takes the contents of a CPU register
      (usually 8-512 bits of data) and writes it into memory.

   Syslog
      Long the *de facto* standard logging standard on UNIX systems,
      syslog is now specified (or more accurately, documented) in IEFT
      `RFC 5424 <https://www.rfc-editor.org/rfc/rfc5424.txt>`_.

   Thread
      Thread is short for *thread of control* and represents an
      on-going execution of a program. It consists of a stack, a set
      of registers, meta data such as its :term:`processor affinity`,
      scheduling policy, priority, and potentially a reference to a process
      context as well.

      There are a number of different types of threads, including:

      * :term:`Kernel threads <Kernel thread>`.
      * :term:`User space threads <User space thread>`.
      * :term:`User mode threads <User mode thread>`.

      Data plane threads may serve in a variety of different roles, for
      example:

      * As an :term:`EAL thread`, usually as a :term:`fast path lcore`.
      * As a :term:`registered <Registered non-EAL thread>` or
	:term:`unregistered <Unregistered non-EAL thread>`
	:term:`control thread`.

   Thread safety
      A function is considered multi-thread (MT) safe, often
      abbreviated to thread-safe, if it may safely be called from
      multiple operating system thread in parallel, without
      threatening program correctness. In particular, a thread-safe
      function is free of race conditions.

   TLS
      In C11, and long before in GNU C, a static or extern storage
      class variable may be declared as being kept in Thread Local
      Storage (TLS). Such variables exists in one copy per thread in
      the process. C11 uses ``thread_local`` to mark a variable thread
      local, but in DPDK the practice is to instead use the GCC
      extensions ``__thread``.

   Top half
      A traditional interrupt service routine (ISR) performs all
      processing caused for a particular hardware interrupt in that
      function. This may include running the RX path of an Ethernet
      driver and higher layers of the network stack, and marking a
      user space process to be run.

      Keeping interrupts disabled for a long time may cause issues if
      other interrupts occur. Keeping the core occuppied some
      particular task which is not strictly required to handle the
      interrupt, but rather is just caused by it, may prevent the core
      from working on some more urgent task.

      To solve those problems, serving an interrupt is divided into
      two parts. One, the *top half*, is the actual ISR, does as
      little as possible, which usually translate to clearing the
      interrupt and deferring the rest of the processing to the
      *bottom half*.

      In Linux, there is a set of different mechanisms that qualifies
      as bottom halves (e.g., soft IRQs and tasklets).

   Transparent huge pages
      Transparent huge pages (THP) is a Linux kernel feature which
      make a non-huge pages aware application use huge pages. The
      process of moving data from regular-sized pages (4kB) to huge
      pages may be so high that it defeat the gains made due a
      reduction in TLB misses.

   UE
      User Equipment (UE) is 3GPP term for a mobile terminal. A UE is
      roughly equivalent of a *host* in a TCP/IP network. To complicate
      things, a UE is also almost always a *host* as well, since the
      mobile network is used as a data link layer for IP.

   Unregistered non-EAL thread
      An unregistered non-EAL thread is an operating system thread
      which not registered with the :term:`EAL`, and thus, for
      example, does not have a :term:`lcore id`. In other words;
      a perfectly normal operating system thread, where no
      special actions or precautions have been taken.

      An unregistered non-EAL thread is created by the fast path
      application, or some non-DPDK library it calls into.

      Unregistered thread may not call DPDK APIs which require the
      caller to have a lcore id. Unregistered thread are often also
      :term:`preemptable <Preemptable thread>`, which further restricts
      what DPDK APIs may be used.

   User plane
      A synonym to :term:`data plane`, commonly used in the context of
      telecommunications networks.

   User mode thread
      A thread which is scheduled and otherwise managed by an user space
      application. The actual execution of a user mode thread always
      happens in the context of a operating system thread.

   User space thread
      A user space thread is an :term:`operating system thread`
      which forms a part of the user space process.

   Vector packet processing
      Vector packet processing is a network stack design pattern,
      where the packets traverse the different layers in network stack
      in batches ("vectors"), rather than as individual packets. The
      implementation-level layers may correlate with the layers of the
      :term:`network protocol suite` being implemented, but may also
      be more fine-grained (e.g., IP processing may be split into two
      or three such "sub layers"), or just different altogether. In a
      traditional network stack, a packet traverse the whole stack up
      until completion (e.g., the packet is dropped, forwarded, or
      handed off to a local application).

      The benefit of vector packet processing is reduced instruction
      cache pressure, and improve temporal locality for data related
      to a particular layer. It also reduces the number of required
      function calls. A drawback is that the reduced readability and
      an increase in code complexity, especially if manual loop
      unrolling is used.

      Besides vector packet processing is passing vectors of packets
      between layers, the sub layer processing code allows the
      compiler to use SIMD instructions to a much higher degree that
      would be possible in a single-packet-per-layer design.

      One prominent use of the Vector packet processing pattern is the
      Open Source network router and switch platform with the same
      name - :term:`VPP`.

   :ref:`VNF <Network Function>`
      A virtualized network function (VNF) is a :term:`network function`
      hosted in a virtual machine.

   Virtual core
      A synonym for :term:`logical core`.

   Virtual function
      See :term:`SR-IOV`.

   VPP
      `Vector Packet Processing <https://fd.io/>`_ (VPP) is a Open
      Source data plane platform, with built-in router and switch
      applications. It optionally uses DPDK for packet I/O, but
      otherwise does not make use of DPDK as a platform.

   Wait-free algorithm
      An wait-free algorithm is kind of :term:`non-blocking algorithm`
      that guarantees that all threads involve make progress.

   Wall-clock latency
      Wall-clock latency, or wall-time latency, is the latency in
      terms of the passage of physical time (i.e., what a wall clock
      measures). A commonly used synonym (e.g., in the context of
      manufacturing) is *lead time*. The wall-clock latency may be
      longer or shorter than the :term:`processing latency`.

   Work scheduler
      For the purpose of this book, a work scheduler (also known as a
      job scheduler) is a data plane fast path function that assign
      :term:`items of work <Item of work>` to the worker lcores. Work
      scheduling in one of its most simple forms is the use of
      :term:`RSS` in the NIC. A DPDK Event Device is a form of work
      scheduler. In a data plane application, a job is usually, but
      not always, processing a packet (at a certain stage in the
      pipeline, or the complete processing, for run-to-completion
      designs).

   Worker lcore
      All :term:`logical cores <Logical core>` assigned to a DPDK
      applications are worker lcores, except the core designated as
      the :term:`main lcore`, and any :term:`services lcores <Service
      lcore>`.

   Working set size
     The amount of memory actively being used by a program, as opposed
     to memory merely allocated, and then left unused. This book will
     used this term to denote *actively used* to mean memory that is
     being repeatedly and frequently accessed, as opposed to memory
     that is only rarely used (e.g., during initialization). The
     reason for this definition is that the primary use for the term
     is in the context of CPU cache pressure. The total amount of
     memory ever used by the application is usually less of a
     concern, for these types of applications. The working set
     includes both instructions and data.
