*****************************
Gramine Device Testing Module
*****************************

A kernel module, implementing a device which can be passed to Gramine to test
all required functionality (e.g shared untrusted memory and ioctl syscall).

.. highlight:: sh

Installing header
=================

The header `gramine_test_dev_ioctl.h` must be installed on the system in order
for userspace programs to use the new IOCTLs. Do it manually:

    sudo cp gramine_test_dev_ioctl.h /usr/local/include/

Now programs may include this header via `#include <gramine_test_dev_ioctl.h>`.

Features
========

This driver implements a custom character device, which allows for simple byte
array manipulations. Each open file handle is associated with a context holding
all necessary data, and allows for the following operations:

- `open` - Create a byte array instance.
- `write` - Write data to the byte array at the current offset. If some data
  was present at this offset, it's overwritten. The byte array is
  automatically extended if needed.
- `read` - Read data from the byte array.
- `llseek` - Change the current offset in the byte array.
- `release` - Releases the byte array instance.
- `unlocked_ioctl` - Perform one of the following IOCTLs:
  - `GRAMINE_TEST_DEV_IOCTL_REWIND` - same as `llseek(filp, 0, SEEK_SET)`.
  - `GRAMINE_TEST_DEV_IOCTL_WRITE` - same as `write`.
  - `GRAMINE_TEST_DEV_IOCTL_READ` - same as `read`.
  - `GRAMINE_TEST_DEV_IOCTL_GETSIZE` - Returns size of the byte array.
  - `GRAMINE_TEST_DEV_IOCTL_CLEAR` - Frees the byte array.
  - `GRAMINE_TEST_DEV_IOCTL_REPLACE_ARR` - Replaces specified characters in
    the byte array; character replacements are passed as an array.
  - `GRAMINE_TEST_DEV_IOCTL_REPLACE_LIST` - same as
    `GRAMINE_TEST_DEV_IOCTL_REPLACE_ARR` but character replacements are passed
    as a NULL-terminated list.
