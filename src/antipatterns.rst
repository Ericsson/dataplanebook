..  SPDX-License-Identifier: CC-BY-4.0
    Copyright(c) 2022 Ericsson AB
    Author: Mattias Rönnblom <mattias.ronnblom@ericsson.com>

Anti Patterns
=============

* ``inline`` is a magical "go faster" keyword that should be applied to all
  functions
* ``static inline`` functions should be used with abundance
* Macros are faster than functions
* volatile is actually useful
* Open-coding spinlocks
* TSO means sequential consistency means I don't have to worry
* Too much in a hurry for modularization
