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

   ATM
      Asynchronous Transfer Mode (ATM) is a type of fixed (wired)
      telecommunications network, that has largely fallen into disuse.
      As opposed to IP, ATM is connection-oriented on the data link layer.

   BGP
      The Border Gateway Protocol (BGP) is a protocol used by Internet
      routers to exchange routing and reachability information.

   Data plane platform
      The part of the data plane applications that provides hardware
      abstractions and the associated hardware drivers, and other
      operating system-like services like work scheduling, memory
      management, and timers. :ref:`DPDK` is the reference data
      plane platform of this book.

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

   Domain logic
      Domain logic, also known as *business logic*, is the part of
      a program that directly correspond to it's core function. For
      a network stack, it's the part the implements the protocol
      processing logic. For example, in an IP stack generating a ICMP
      Time Exceeded when the Time to Live (TTL) has reached 0 is
      domain logic. The part of the stack responsible to retrieve
      the packet via the Ethernet driver is not.

   DPU
      A Data Processing Unit (DPU) is processor designed for data
      plane applications. A DPU is built around a complex of
      general-purpose CPU cores, augmented by networking-specific
      accelerators and high performance network I/O interfaces.
      Older processor generations with the same basic architecture
      was referred to as communication processors.

   Exception traffic
      Exception traffic consists of packets, which during normal
      network conditions are unusual, that for some reason need more
      complex processing. For flow-based forwarding engines, this
      could be the first packet in a previously unseen flow, and as
      such requires checking against security policies and the
      installation of a new entry in the fast path's forwarding
      database. It may also be an ARP request, or a fragmented IP
      packet, for a limited-feature fast path IP stack.

   False sharing
      False sharing occurs when multiple CPU cores accesses two or
      more pieces of logically disjoint data resides on the same CPU
      cache line. For false sharing to have any detrimental effects,
      at least one core need to write to the cache line. The effect is
      a performance degradation, the size of which depends on the
      frequency of access. False sharing does not affect the
      correctness of the program.

   :ref:`Fast path <Fast Path>`
      The data plane fast path is part of the data plane application that
      handles the bulk of the packets.

   FIB
      A Forward Information Base (FIB) holds information on where to
      forward a packet.

   Flow cache
      A flow cache is a data structure which is logically an overlay
      on top of the complete :term:`FIB`. Systems that employ a
      flow cache avoid having to perform a potentially costly FIB lookup
      (among other processing, such as :term:`ACL` lookup operations) for
      every packet in a flow.

   Forwarding plane
      A synonym to data plane, often used for in the context of switches
      and IP router implementations.

   Hardware threading
      In a CPU core employing hardware threading, such as a
      :term:`SMT` system, has two or more hardware threads. From a
      software point of view, each such hardware thread looks just
      like a (virtual) CPU core, with its own set of registers, a
      stack, etc. However, on the level of the physical
      implementation, each hardware thread share, to a varying degree,
      underlying CPU core resources (e.g., core-private caches,
      instruction decoders, or arithmetic logic units) with one or
      more hardware threads on the same core. Such co-located threads
      are usually referred to as siblings threads or just siblings.

      The number of hardware threads is fixed, and unlikely their
      software counter parts, hardware threads do not migrate across
      physical CPU cores. Virtual thread would have been a more
      appropriate term for a hardware thread.

   High touch application
      A data plane fast path application that on average spends relatively
      many CPU clock cycles and other hardware resources for every packet.

   Huge Pages
      The virtual address space is divided into pages, usually 4 kB
      in size. The hardware keeps a cache of translation between
      virtual and physical in a Translation Look-aside Buffer (TLB).
      For applications accessing a large amount of memory (i.e., with
      a large working set size), the TLB cache may be missed, causing
      expensive traps to the kernel. Increasing the page size for
      part of the virtual memory is a way to avoid this issue. Such
      pages are often very much large (e.g., 2 MB or 1 GB), and thus
      are often referred to as "huge pages".

   ISA
      An Instruction Set Architecture (ISA) specifies the interface
      between software and the CPU hardware. The ISA defines things like
      the available machine language instructions (and how they
      are encoded), registers, data types and memory models.

   Layer 2
      The data link layer is the second layer in OSI model, and handles
      data transmission between different nodes on the same physical
      network segment. Ethernet is an example of a layer 2 data link
      layer protocol.

   Lcore
      Short for logical core. Lcore is a term extensively used in DPDK,
      and means something that looks like a CPU core from a software
      point of view, and is allocated to the DPDK application (via a core
      mask). In a bare metal system, a logical core is either a "full"
      physical core (for non-SMT systems) or a hardware thread (for cases
      where hardware threading is employed).

   Lcore worker thread
      An operating system software thread responsible for processing packets
      for the data plane fast path. This thread is pinned to a particular
      by DPDK, and should be the only thread scheduled on that lcore.

   Load
      A load machine instruction reads a chunk of data (usually 8-512
      bits) from memory and puts it into a CPU register.

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

   MIB
     A Management Information Base (MIB) is a :term:`SNMP` data model.
     The term is sometimes also used to refer to an instance of a
     particular model. IEFT has defined a number of MIBs (e.g., for
     TCP and IP).

   :ref:`Management plane <Management Plane>`
      The part of the network that handles configuration and
      monitoring.

   NAT
      Network Address Translation (NAT) is a method of rewriting the
      IP packet header to translate to change the source and/or
      destination host and/or port, often for the purpose of having
      multiple IP hosts to between host's and its single IP address.

   NETCONF
      The Network Configuration Protocol (NETCONF) is an XML-based
      network configuration management protocol developed by the IEFT.

   Network function
      For the purpose of this book, the immediate surroundings to the
      data plane application, which work in concert to provide a data
      plane function to interface with the control plane and other
      instances of data plane functions.

   Network protocol suite
      A set of related communication protocols, usually arranged in
      layered architecture, used in a computer network.

   Network stack
      A network stack, also known as a protocol stack, is an
      implementation, usually in software, of a family or
      :term:`suite<network protocol suite>` of network protocols.

   Mythical Man-Month
      In the book titled *The Mythical Man-Month: Essays on Software
      Engineering*, Fredrick Brooks of IBM debunks the myth that a
      software project can be estimated in man-months. In particular,
      he observes that the communication overhead grows in non-linear
      fashion as people are added to the project.

   Parallelism
      The term parallel, as used in this book, is reserved for
      situations when two or more tasks are literally performed during
      the same, or at least overlapping, time period. To result of
      various time sharing schemes (e.g., multitasking or temporal
      hardware threading), the term :term:`concurrency` is used
      instead.

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

   Preemption Safety
      A operation is preemption safe in case the preemption of a
      thread's execution (e.g., a kernel-induced process context
      switch occurs) does not threaten the correctness of the program,
      or have very detrimental effects performance. In this book, the
      preemption unsafe constructs covered only cause performance
      degradation, although at time very serious such.

   Processing Latency
      For the purpose of this book, processing latency is the CPU time
      spent on a particular task (i.e., the number of CPU core
      cycles).  In case the processing is performed on multiple cores
      in parallel, the processing latency may be greater than the
      :term:`wall-clock latency`. In case a packet is buffered (e.g.,
      on the NIC), and the data plane CPU cores are very busy, the
      processing latency may be only a small fraction of the total
      port-to-port wall-clock latency experience by that packet.

      In the context of IP routers, the term is used to denote all
      latency that occurs within the router (i.e., both CPU related
      latency and internal queuing latency). This is not how the term
      is used in this book.

   Program order
      Operations are said to be done in program order if their results
      are globally visible in the same order as the operations were
      specified in the program's source code.

   RAN
      The Radio Access Network (RAN) is the network that sits between
      the :term:`UE` and the :term:`CN` in a mobile telecommunications
      system.

   RFS
      See :term:`RSS`.

   RSS
      Receive Side Scaling. A NIC function which distributes packets
      to different NIC RX descriptor queues, usually based on the
      source and destination IP. If transport layer fields are taken
      into a account, the same function is sometimes called
      Receive Flow Scaling (RFS).

   Sequence counter
      A sequence counter is a low-overhead reader-writer synchronization
      mechanism.

   Slow path
      The part of a data plane application that process exception traffic.

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

   TLS
      In C11, and long before in GNU C, a static or extern storage
      class variable may be declared as being kept in Thread Local
      Storage (TLS). Such variables exists in one copy per thread in
      the process. C11 uses ``thread_local`` to mark a variable thread
      local, but in DPDK the practice is to instead use the GCC
      extensions ``__thread``.

   UE
      User Equipment (UE) is 3GPP term for a mobile terminal. A UE is
      roughly equivalent of a *host* in a TCP/IP network. To complicate
      things, a UE is also almost always a *host* as well, since the
      mobile network is used as a data link layer for IP.

   User plane
      A synonym to :term:`data plane`, commonly used in the context of
      telecommunications networks.

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

   VPP
      `Vector Packet Processing <http://fd.io/>`_ (VPP) is a Open
      Source data plane platform, with built-in router and switch
      applications. It optionally uses DPDK for packet I/O, but
      otherwise does not make use of DPDK as a platform.

   Wall-clock Latency
      Wall-clock latency, or wall-time latency, is the latency in
      terms of the passage of physical time (i.e., what a wall clock
      measures). A commonly used synonym (e.g., in the context of
      manufacturing) is *lead time*. The wall-clock latency may be
      longer or shorter than the :term:`processing latency`.

   Work scheduler
      For the purpose of this book, a work scheduler (also known as a
      job scheduler) is a data plane fast path function that assign
      items of work to the worker lcores. Work scheduling in one of its
      most simple forms is the use of :term:`RSS` in the NIC. A DPDK
      Event Device is a form of work scheduler. In a data plane
      application, a job is usually, but not always, processing a
      packet (at a certain stage in the pipeline, or the complete
      processing, for run-to-completion designs).

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
