*****************************
Gramine Device Testing Module
*****************************

A kernel module, implementing a device which can be passed to Gramine to test
all required functionality (e.g shared untrusted memory and ioctl syscall).

.. highlight:: sh

Features
========

This driver implements a custom character device, which allows for simple string
manipulations. Each open file handle is associated with a context holding all
necessary data, and allows for the following operations:

- `open` - Create a string instance.
- `write` - Write data to the string at the current offset. If some data was
  present at this offset, it's overwritten. The string is automatically
  extended if needed.
- `read` - Read that from the string.
- `llseek` - Change the current offset in the string.
- `release` - Releases the string instance.
