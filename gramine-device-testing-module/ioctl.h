#ifndef IOCTL_H
#define IOCTL_H

#include <linux/ioctl.h>

struct gramine_ioctl_write {
    size_t buf_size;        /* in */
    const char __user* buf; /* in */
    loff_t off;             /* in/out -- updated after write */
    ssize_t copied;         /* out -- how many bytes were actually written */
};

struct gramine_ioctl_read {
    size_t buf_size;        /* in */
    char __user* buf;       /* out */
    loff_t off;             /* in/out -- updated after read */
    ssize_t copied;         /* out -- how many bytes were actually read */
};

struct gramine_ioctl_replace_char {
    char src;               /* in */
    char dst;               /* in */
    char pad[6];
    struct gramine_ioctl_replace_char __user* next; /* unused for array, used for list */
};

struct gramine_ioctl_replace_arr {
    /* array of replacements, e.g. replacements_cnt == 2 and [`l` -> `$`, `o` -> `0`] */
    size_t replacements_cnt;
    struct gramine_ioctl_replace_char __user* replacements_arr;
};

struct gramine_ioctl_replace_list {
    /* list of replacements, e.g. [`l` -> `$`, next points to `o` -> `0`, next points to NULL] */
    struct gramine_ioctl_replace_char __user* replacements_list;
};

#define GRAMINE_IOCTL_BASE 0x33

#define GRAMINE_IOCTL_REWIND        _IO(GRAMINE_IOCTL_BASE, 0x00)
#define GRAMINE_IOCTL_WRITE       _IOWR(GRAMINE_IOCTL_BASE, 0x01, struct gramine_ioctl_write)
#define GRAMINE_IOCTL_READ        _IOWR(GRAMINE_IOCTL_BASE, 0x02, struct gramine_ioctl_read)
#define GRAMINE_IOCTL_GETSIZE       _IO(GRAMINE_IOCTL_BASE, 0x03)
#define GRAMINE_IOCTL_CLEAR         _IO(GRAMINE_IOCTL_BASE, 0x04)
#define GRAMINE_IOCTL_REPLACE_ARR  _IOW(GRAMINE_IOCTL_BASE, 0x05, struct gramine_ioctl_replace_arr)
#define GRAMINE_IOCTL_REPLACE_LIST _IOW(GRAMINE_IOCTL_BASE, 0x06, struct gramine_ioctl_replace_list)

#endif
