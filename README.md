# OpenRISC 1000 Multicore Virtual Platform (or1kmvp)

This repository contains the source code of `or1kmvp`, an OpenRISC 1000
Multicore Virtual Platform based on SystemC/TLM. It models a regular
symmetric multiprocessor design with a configurable number of processors and
a set of I/O peripherals. `or1kmvp` is detailed enough to run the Linux
kernel and fast enough to allow interactive command-line use.
Depending on the host computer, this full system simulator reaches a total
performance of around 35 MIPS (which are split among all instruction set
simulators present in the system).
It is intended to be used by software developers to test and run parallel
OpenRISC applications without the need for physical OpenRISC hardware.
Furthermore, it can also be used as a starting point for custom SystemC
and OpenRISC based Virtual Platforms.

----
## Build & Installation
todo

----
## Operation
To start simulation, you can run
```
<install-dir>/bin/or1kmvp -f <install-dir>/config/[up.cfg|smp2.cfg]
```
The `up.cfg` constructs a uniprocessor system, while `smp2.cfg` yields a
dual core system. Note that, if you have built `or1kmvp` directly from source,
you will additionally need Linux kernel images, device tree blobs and an
initrd and place them in `<install-dir>/sw`. These files are referenced by
`up.cfg` and `smp2.cfg`, so you need to name them accordingly.

----
## Memory Map
This is the memory map of the system. Kernel device trees must be kept in sync
with this.

```
0: 0x00000000 .. 0x07ffffff -> memory
1: 0x90000000 .. 0x90001fff -> uart8250
2: 0x98000000 .. 0x98001fff -> ompic
```
Note that the default configuration for this system comes with 128MB of memory.
You can increase this using the `system.mem.size` property (either via a
config file or via the `-c` command line switch). If you change memory size,
the default memory map will automatically adjust.