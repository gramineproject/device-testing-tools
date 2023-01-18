#!/bin/sh

set -e

# the `-append ... $*` snippet can't handle environment variables with spaces in them, but kernel
# args (host paths and 0/1 values) are not supposed to have spaces anyway
exec qemu-system-x86_64 \
    -enable-kvm \
    -kernel /boot/vmlinuz-$(uname -r) \
    -initrd initramfs.cpio.gz \
    -nographic \
    -monitor /dev/null \
    -cpu host \
    -smp 2 \
    -m 1G \
    -append "console=ttyS0 loglevel=3 quiet oops=panic $*" \
    -device virtio-rng-pci \
    -virtfs 'local,path=/,id=hostfs,mount_tag=hostfs,security_model=none,readonly=on' \
    -device 'virtio-9p-pci,fsdev=hostfs,mount_tag=hostfs' \
    -object memory-backend-epc,id=epc_mem,size=64M,prealloc=on \
    -M sgx-epc.0.memdev=epc_mem
