iCEKeeb gateware & firmware
===========================

This repository contains the gateware and firmware for the iCEBreaker-bitsy powered FPGA keyboard.

This is a research project to find out what the advantage is of using an FPGA instead of a hard IP microcontroller.

So far we have identified the following possible advantages that we are working on implementing and validating:
* Ability to implement atypical key matrix scanning patterns in hardware. For example, *binary search* or *high wait until key press* approaches.
* Ability to implement arbitrary protocols beyond USB in hardware. For example natively support PS/2, AT, ADB or Amiga keyboard protocols.
* Possibly improved latency.

Hopefully more ideas will emerge as we implement more of the system.

System Architecture
===================
The iCEBreaker-bitsy comes already with a DFU bootloader that allows us to upload the bitstream and RISC-V soft core firmware into the iCEBreaker-bitsy FLASH.

The keyboard gateware and firmware is derived from the iCEBreaker-bitsy bootloader that was designed and implemented by @tnt.

The gateware is based upon a picorv32 RISC-V soft core, with several IP cores connected over the wishbone bus.
* USB
* debug UART
* RGB LED
* keyscanner

The firmware is currently written in C, but @esden is dreaming of porting it to rust one day. ;)

How to build and program
========================

To build and flash the gateware and firmware on an iCEBreaker-bitsy follow the instructions in [iCEKeeb README](gateware/icekeeb/README.md).


License
=======

Gateware is mostly under "CERN Open Hardware license v2 Permissive", but some of the cores are under other permissive licenses. Each file has the license specified in its header.

Firmware is mostly under LGPL3+, against some files imported from other places are under more permissive licenses. Each file has the license specified in its header.

Some other files/cores imported through submodules might differ from the above but they are not used / don't end up in the final build, that's just the nature of submodules.

In case of submodules, always refer to the README/LICENSE of the submodule itself for more details.
