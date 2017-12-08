#!/bin/bash
qemu-system-i386 -drive file=dst/dbg/img,format=raw -no-reboot -no-shutdown -monitor stdio -d int,cpu_reset,unimp,guest_errors
