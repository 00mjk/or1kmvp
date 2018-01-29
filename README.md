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

In order to build `or1kmvp`, you need the following prerequisite software
libraries:
* `SystemC`: the underlying simulation engine
* `or1kiss`: the OpenRISC 1000 instruction set simulator
* `vcml`: a SystemC addon library with TLM models for peripherals etc.
* `libelf`: for reading ELF binaries
* `cmake`: is used as the build system

These libraries should have guides on how to build them that take precedence
over this one. However, for convenience, this is the full build procedure:

1. Install `libelf` and `cmake`:
    ```
    sudo apt-get install cmake libelf-dev  # Ubuntu
    sudo yum install cmake3 libelf-dev     # Centos
    ```

2. Download and build SystemC. Make sure to set the environment variables
   `SYSTEMC_HOME` and `TARGET_ARCH` afterwards:
    ```
    wget http://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.1a.tar.gz
    tar -xzf systemc-2.3.1a.tar.gz && cd systemc-2.3.1a
    export SYSTEMC_HOME=`pwd`
    export TARGET_ARCH=linux64
    mkdir BUILD && cd BUILD
    ../configure --prefix=$SYSTEMC_HOME --enable-optimize --enable-static CXXFLAGS="-std=c++11"
    make -j 4 && make install
    ```

3. Clone and build `vcml`. Make sure to set the environment variable
   `VCML_HOME` afterwards. `vcml` requires SystemC, but `cmake` should be
   able to locate it automatically if you have set `SYSTEMC_HOME` as indicated
   in the previous step:
   ```
   git clone https://github.com/janweinstock/vcml.git && cd vcml
   mkdir -p BUILD/RELEASE/BUILD && cd BUILD/RELEASE/BUILD
   cmake ../../.. -DCMAKE_INSTALL_PREFIX=.. -DCMAKE_BUILD_TYPE=RELEASE
   make -j 4 && make install
   export VCML_HOME=`pwd`/..
   ```

4. Clone and build `or1kiss`. Make sure to set the environment variable
   `OR1KISS_HOME` afterwards as indicated below:
   ```
   git clone https://github.com/janweinstock/or1kiss.git && cd or1kiss
   mkdir -p BUILD/RELEASE/BUILD && cd BUILD/RELEASE/BUILD
   cmake ../../.. -DCMAKE_INSTALL_PREFIX=.. -DCMAKE_BUILD_TYPE=RELEASE
   make -j 4 && make install
   export OR1KISS_HOME=`pwd`/..
   ```

5. Once the prerequisites are done, we can start building `or1kmvp`. First
   chose directories for building and deployment:
    ```
    <source-dir>  location of your repo copy,     e.g. /home/jan/or1kmvp
    <build-dir>   location to store object files, e.g. /home/jan/or1kmvp/BUILD
    <install-dir> output directory for binaries,  e.g. /opt/or1kmvp
    ```

6. Configure and build the project. If you get an error, make sure the
   following environment variables are set correctly:

   * `SYSTEMC_HOME` (see step 2)
   * `TARGET_ARCH`  (see step 2)
   * `VCML_HOME`    (see step 3)
   * `OR1KISS_HOME` (see step 4)

   Configuration is done with `cmake` using the regular parameters:
   ```
   mkdir -p <build-dir>
   cd <build-dir>
   cmake <source-dir> -DCMAKE_INSTALL_PREFIX=<install-dir> -DCMAKE_BUILD_TYPE=RELEASE
   make -j 4 && make install
   ```

7. After installing, the following new files and directories should be present:
   ```
   <install-dir>/bin/or1kmvp # simulator binary
   <install-dir>/config/     # directory holding example configuration files
   ```
   Note that in order to boot Linux, you will additionally need the kernel
   image, a compiled device tree and an initrd. These large binary files are
   not included in this repository, but can be found in the release list of
   this project.

----
## Operation
To start simulation, you can run
```
<install-dir>/bin/or1kmvp -f <install-dir>/config/[up.cfg|smp2.cfg]
```
The `up.cfg` constructs a uniprocessor system, while `smp2.cfg` yields a
dual core system. Note that, if you have built `or1kmvp` directly from source,
you will additionally need Linux kernel images, compiled device tree files and
an initrd and place them in `<install-dir>/sw`. These files are referenced by
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

----
## License

This project is licensed under the Apache-2.0 license - see the
[LICENSE](LICENSE) file for details.