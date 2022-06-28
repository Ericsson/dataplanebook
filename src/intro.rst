..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

Introduction
************

This text is an attempt at a guide to the structure and implemention
of data plane applications. The focus lies on the part of the software
which handles the bulk of the packets, often with a very tight latency
budget. The book is about life in the fast path.

The applications of this book usually implements some kind of
networking stack. Although the book anchored them in the Data Plane
Development Kit (DPDK) platform, most patterns and principles apply
more widely, such as a networking stack of an operating system kernel.

As with all software architecture, it's easier to grasp a pattern or a
principles if it's anchored in something concrete. The concrete here
is the DPDK APIs.

A module of data plane application will more resemble that of a module
inside an monolithic kernel design such as Linux, rather than an
application sitting on top of the UNIX system call boundary.

.. index::
   Data Plane: The part of the network that handles that actual user data

The Data Plane
==============

The *data plane* is the part of the network through which the actual
user data traverse. A *user* here is an application on some end system
(in a TCP/IP network, a *host*).

Synonyms used are *forwarding plane*, primarily in the context of IP
routers, or *user plane*, often in a telecommunications context.

Data Plane Development Kit
==========================

The `Data Plane Development Kit <http://www.dpdk.org/>`_ (DPDK) is an
Open Source software platform for data plane applications. DPDK comes
in the form of a set of link libraries, accessed through C APIs.

The initial design was made by the Intel engineer Venky Venkatesan.

A DPDK-based application runs as a user space process.

The original purpose of DPDK was to allow the user to bypass the
operating system's networking stack, avoiding a substantial amount of
overhead. In addition, being outside the kernel, DPDK avoided the some
licensing issues, allowing for properitary application networking
applications on Linux. It also changed out the challenging programming
environment of the kernel, with something that - at least at a first
glance - is less challenging.

To allow for the kernel bypass, the Network Interface Controller (NIC)
hardware (or a virtual function thereof) is mapped in the process'
address space. Instead, or in addition to, the kernel's NIC driver,
the application uses a driver provided by DPDK, for that particular
hardware. In addition to the NIC driver, a packet buffer memory
manager is needed as well, but not much else in terms of
infrastructure.

This "for-I/O only" way of using DPDK is still common place today.

A non-trivial DPDK application will, as its functionality grows, often
become to resemble a special-purpose network stack, with some
kernel-like support functionality (e.g., for timer management and work
scheduling).

With time, DPDK has included more and more of this kind of reusable
support facilities. DPDK still largely adheres to the original library
approach, as opposed to becoming an integrated special-purpose
operating system for data plane applications.

DPDK was originally developed for custom off the shelf (COTS) desktops
and servers, but has worked itself into the world of networking ASICs
as well, where the traditional network processing unit (NPU) have been
replaced with a general-purpose multi-core CPU complex, augemented
with various networking-specific accelerators and occassionally CPU
core instruction set architecture (ISA) extensions as well.

DPDK has largely dislodged the Open Data Plane (ODP). ODP is a project
with a similar goal, but which was closer to the non-x86 and network
communications vendors eco systems. DPDK lacked in the area of support
for various accelerators that were found in such processors, but has
gradually caught up with ODP.

Data Plane Applications
=======================

For the purpose of this book, a *data plane application* is a piece of
software which implements a part of the data plane. At the core of the
data plane application is some sort of a *network stack* (also known
as a *protocol stack*).

This book concerns itself with applications written in a
general-purpose programming language (e.g., C), run on a symmetric
multi-processing (SMP) computer processor.

A data plane application sits on top of a data plane platform. This
book assumes the division of concern between the platform and the
application to be that of DPDK, as opposed to a more opinionated
platform like `Vector Packet Processing <http://fd.io/>`_ (VPP).

Generally, the data plane has a tight per-packet hardware resource
(e.g., CPU core cycles or DDR bytes read/written per packet) and/or
energy (i.e., Joule/packet) budget, often in the from a couple of
hundred to a couple of thousands cycles spent in total per packet.

A typical characteristic of the data plane application is that the
core functionality - the *domain logic* - is not very complex, in
comparison with for example a desktop or enterprise application.

An application which performs very little per-packet processing is
often referred to as *low touch*. The term implies only a small part
of the packet, usually the header, is ever visited, but this book will
not make this limitation, but only use the term to suggest it uses
comparable few CPU core cycles to finish its job.

A *high touch* application has a relatively high per-packet processing
latency. *High touch* application often read the whole packet,
including the payload. For the purpose of this book, even the high
touch application is "low-touch" enough to benefit from avoiding the
operating system kernel.

In summary, a data plane application needs to do something reasonably
simple, but very quickly and efficiently

Virtual switches and routers, deep packet inspection, firewalls, NAT
boxes, network attached storage (NAS), intrusion detection systems, 5G
core and radio access network (RAN) implementations, and L4-L7 load
balancers as all examples of system which can be realized with one or
more data plane applications.

Low Latency Non Data Plane Applications
---------------------------------------

There are other classes of applications, for example low-latency
financial trading applications, that, even though they are not a part
of the implementation of the network, still are similar enough to make
use of similiar technologies and techniques.

Indeed, any application that will receive stimuli over a network, and
very quickly and/or very oftenly must provide a response of some sort,
may fit under the characteristics of a data plane application,
although in the literal sense they are not.

Low Capacity Data Plane
-----------------------

The are data plane applications that have comparably generous budgets,
both in terms of throughput, latency and efficient use of computer
hardware. Thus, they can avoid the cumbersome and challenging
techniques of this book.

Such an application could well use general-purpose operating system
networking stack. Its author has much more freedom when it comes to
the choice of programming languages and program structure.

On the other hand, such applications have a tendence to have a much
higher complexity in terms of the actual functionality required.

Such applications are out of the scope of this book.

Hardware Platforms
==================

Processor
---------

This books concerns itself with software for the kind of processor
that is capable, at least in principle, to run a general-purpose
operating system.

The processors are symmetric multi-processors (SMPs).

The processor can be general-purpose server processor, or something
esentially functionally equivalent, but hosted inside a specialized
networking system-on-a-chip (SoC).

DPDK support the most relevant ARM instruction set architectures
(ISAs), AMD64 (or IA-64 in Intel terminology), or the Power ISA of the
IBM POWER9 family, and RISCV. Usually, the ISA is generic in the sense
it has no special networking-related instructions added to it compared
to other implementations, there is nothing in the DPDK model that
won't prevent such additions.

Accelerators
------------

Memory
------

People
======

Why Data Plane Programming is Easy
----------------------------------

Data plane software development, generally speaking, gives the
programmer an easy problem, from a domain logic point of view, to
solve in a demanding environment, with demanding characteristics
requirement.

Why Data Plane Programming is Hard
----------------------------------

One reason why writing data plane code is hard because
high-performance, parallel programming is hard. C, which is generally
still the language of choice in this domain, is no big help here, but
also is kind enough to not be in the way. Despite C's use in operating
system kernels, it didn't get a proper memory model until in ISO/IEC
9899:2011 (C11).

Data plane programming is also hard because of the immense complexity
of modern CPUs. Their behaviour very difficult indeed to understand,
or to predict, both in performance and functional correctness (e.g.,
memory models).

As a result, debugging is also harder. Low-level hardware
characteristics that the software developer in many cases need not
care about in other application domains.

Data plane development is one of the few domains, together with
operating system kernels, virtual machines and compilers, where still
reasons for the developer to understand and occasionally even write
assembler code.

Understanding the C11 memory model and lock-less programming, which is
often useful or even essential, is also notiously difficult. Bugs in
such code is very difficult to test for.

Staffing Data Plane Projects
----------------------------

In essence, the typical data plane software project takes on a small
task - in terms of man hours - but a difficult one. If manned with
people without the appropriate skills, but attempts are made to
compensate with more people, this

An attempt to compesate a team's lack of skill with higher head count
can easily become fatal. This approach risk project's code base from
small, nimble and performant application, to a huge, slow, beast,
which main characterist is it's ability to display non-deterministic
behavior.

Well-crafted data plane programs are often challenging to understand.
Poorly written data plane code is beyond human capabilities to
decipher.

Data plane applications tend to be in the range from ten thousand to a
couple of hundred thousand lines of code, and thus cannot accomodate a
large team. Also, such applications tend to have higher coupling
between the different parts, which further restricts the team size.

Data plane software development requires highly specialized skills and
is also very cognitively demanding. That's the downside. The upside is
that you don't need very many people.

If there's anywhere the Mythical Man-Month applies, it's in the
context of data plane software development.
