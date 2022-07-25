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
  - `unlocked_ioctl` - Perform one of the following IOCTLs:
    - `GRAMINE_TEST_DEV_IOCTL_REWIND` - same as `llseek(filp, /*offset=0*/0, SEEK_SET)`.
    - `GRAMINE_TEST_DEV_IOCTL_WRITE` - same as `write`.
    - `GRAMINE_TEST_DEV_IOCTL_READ` - same as `read`.
    - `GRAMINE_TEST_DEV_IOCTL_GETSIZE` - Returns size of the string.
    - `GRAMINE_TEST_DEV_IOCTL_CLEAR` - Frees the string.
    - `GRAMINE_TEST_DEV_IOCTL_REPLACE_ARR` - Replaces specified characters in the string;
      character replacements are passed as an array.
    - `GRAMINE_TEST_DEV_IOCTL_REPLACE_LIST` - same as `GRAMINE_TEST_DEV_IOCTL_REPLACE_ARR`
      but character replacements are passed as a NULL-terminated list.
