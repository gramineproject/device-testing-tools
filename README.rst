*****************************************
Testing device passthrough in Gramine SGX
*****************************************

Just a set of tools and scripts designed for testing host device passthrough in
Gramine SGX PAL.

.. highlight:: sh

Prerequisites
=============

In order to run Gramine SGX PAL inside QEMU, you need both QEMU and host kernel
to support SGX. The support was added in Linux v5.15 and QEMU v6.2.0.
Ubuntu 22.04 comes with both in appropriate versions, so it's the easiest to
just use it.

QEMU
====

In case QEMU provided by your distribution does not support SGX, you can build
it with the scripts we provide. Just do::

    cd qemu
    make

The resulting binary will be symlinked into the main directory.

Initramfs builder
=================

Set of scripts to build and run the virtual machine. Uses host kernel for
simplicity. Whole host filesystem ("/") is mounted readonly at root with
a read-write overlay in tmpfs (volatile in memory). This is done for simplicity,
so that we don't have to build Gramine inside VM, provide all dependencies, etc.

Gramine Device Testing Module
=============================

Implemenation of a dummy kernel module, implementing a device which can be
passed to Gramine to test all required functionality (in particular shared
untrusted memory and ioctl syscall).

Other issues
============

Currently Gramine (actually glibc 2.34) does not build on Ubuntu 22.04. You may
use `gramine_ubuntu22.04.patch` to fix that.
