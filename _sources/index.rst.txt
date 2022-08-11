..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

##########################
Data Plane Software Design
##########################

.. note::
   What you see here is only very early draft of this book. It currently
   consists of a skeleton structure, plus two chapters
   :ref:`Introduction` and :ref:`Statistics` in draft versions.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   intro
   threading
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
   resource sharing and power management
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
   Non-DPDK frameworks (e.g., fd.io VPP)

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
