#!/bin/bash
qemu-system-i386 -drive file=dist/img,format=raw -no-reboot -no-shutdown -monitor stdio -d int,cpu_reset,unimp,guest_errors
