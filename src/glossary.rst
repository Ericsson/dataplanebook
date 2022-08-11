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

   ATM
      Asynchronous Transfer Mode (ATM) is a type of fixed (wired)
      telecommunications network, that has largely fallen into disuse.
      As opposed to IP, ATM is connection-oriented on the data link layer.

   BGP
      The Border Gateway Protocol (BGP) is a protocol used by Internet
      routers to exchange routing and reachability information.

   Data plane platform
      The part of the data plane applications that provides hardware
      abstractions and the assoicated hardware drivers, and other
      operating system-like services like job scheduling, memory
      management, and timers. :ref:`DPDK` is the reference data
      plane platform of this book.

   Communications Processor
      An older name for a :term:`DPU`.

   :ref:`Control plane <Control Plane>`
      The part of the network that negiotates, computes or otherwise handles
      higher-level policies, such as how routing is set up, and makes sure
      they take affect in the Data Plane.

   :ref:`Control thread <Control Threads>`
      A control thread is a thread running as a part fast path process,
      responsible for serving process-external interfaces, translating
      requests into calls into the fast path's internal APIs. Unlike
      their lcore worker thread counterparts, the control threads usually
      don't run on dedicated CPU cores.

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

   Domain logic
      Domain logic, also known as *business logic*, is the part of
      a program that directly correspond to it's core function. For
      a network stack, it's the part the implements the protocol
      processing logic. For example, in an IP stack generating a ICMP
      Time Exceeded when the Time to Live (TTL) has reached 0 is
      domain logic. The part of the stack responsible to retrieve
      the packet via the Ethernet driver is not.

   Fast path
      The part of the data plane application that handles the bulk of
      the packets.

   FIB
      A Forward Information Base (FIB) holds information on where to
      forward a packet.

   Flow cache
      A flow cache is a data structure which is logically an overlay
      on top of the complete :term:`FIB`. Systems that employ a
      flow cache avoid having to perform a potentially costly FIB lookup
      (among other processing, such as :term:`ACL` lookups) for every
      packet in a flow.

   Forwarding plane
      A synonym to data plane, often used for in the context of switches
      and IP router implementations.

   High touch application
      A data plane fast path application that on average spends relativily
      many CPU clock cycles and other hardware resources for every packet.

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

   Low touch application
      A data plane fast path application that on average spends relativily
      few CPU clock cycles and other hardware resources for every packet.

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
      implemention, usually in software, of a family or
      :term:`suite<network protocol suite>` of network protocols.

   Mythical Man-Month
      In the book titled *The Mythical Man-Month: Essays on Software
      Engineering*, Fredrick Brooks of IBM debunks the myth that a
      software project can be estimated in man months. In particular,
      he observes that the communcation overhead grows in non-linear
      fashion as people are added to the project.

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

   RAN
      The Radio Access Network (RAN) is the network that sits between
      the :term:`UE` and the :term:`CN` in a mobile telecommuncations
      system.

   Slow path
      The part of a data plane application that process exception traffic.

   SNMP
      The Simple Network Management Protocol is a network management
      protocol for IP networks. Originally intended for configuration
      management, current-day use is primarily for network monitoring.

   Spinlock
      A type of lock where a thread that attempts but fails to acquire
      a lock immediately retries again, and keeps doing so ("spins"),
      until the lock operation is successful. This sort of lock is common
      in operating systems kernels, but unusual in regular user space
      applications, because of issues related to preemption safety.

   SSH
      Secure Shell (SSH) is a protocol for remote shell access and
      command execution. It may also be used as a secure transport
      layer (e.g., for :term:`NETCONF`).

   Syslog
      Long the de facto standard logging standard on UNIX systems,
      syslog is now a specified (or rather, documented) in IEFT
      `RFC 5424 <https://www.rfc-editor.org/rfc/rfc5424.txt>`_.

   UE
      User Equipment (UE) is 3GPP term for a mobile terminal. This is
      the equivalent of a *host* in a TCP/IP network.

   User plane
      A synonym to :ref:`Data Plane`, commonly used in the context of
      telecommunications networks.

   Wall-clock Latency
      Wall-clock latency is the latency in terms of the passage of
      physical time (i.e., what a wall-clock measures).
