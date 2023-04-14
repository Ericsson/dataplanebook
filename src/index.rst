..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

##########################
Data Plane Software Design
##########################

.. note::
   What you see here is only a very early draft of this book. It
   consists of a skeleton structure, and three chapters in draft
   form: :ref:`Introduction`, :ref:`Threading` and :ref:`Statistics`.

.. toctree::
   :maxdepth: 3
   :caption: Contents:

   intro
   threading/threading
   work
   eth
   mbuf
   headers
   mem
   sync
   cache
   datastructures
   stats/stats
   time
   timers
   crypto
   modularization
   control
   slowpath
   antipatterns
   glossary

.. pipeline organization and work scheduling
   hardware accelerator and NIC accessing techniques (e.g., batching requests)
   vpp depth first or bredth first (pipeline-internal organization)
   dma devices
   target hardware
   resource sharing and power management, energy efficiency
   platform abstraction layers
   tracing
   portability
   dynamic behavior observability
   c language considerations
   inlining
   constant propagation
   libdivide
   driver/api division of concern
   SMT
   DVFS and sleep states
   Batch and vector processing
   NUMA
   False Sharing
   Cache partitioning
   Compiling and Linking (incl LTO)
   eBPF uses
   non-DP apps; short transactions, low latency apps with DPDK
   traffic management
   terminology
   benchmarking and profiling
   virtualization
   cloud considerations
   scaling
   non-DPDK frameworks (e.g., fd.io VPP)
   overload handling and backpressure
   cryptography
   security

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
