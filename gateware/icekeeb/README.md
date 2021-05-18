RISC-V core for FPGA mechanical keyboards
=========================================

For the iCEBreaker-bitsy, the hardware connections are :
  * `42`: USB DP
  * `38`: USB DN
  * `37`: Pull up. Resistor of 1.5 kOhm to USB DP 

To run :
  * Build and flash the bitstream
      * `make dfuprog`
      * This will build `fw/boot.hex` and include it as the BRAM initial data

  * Flash the main application code in SPI at offset 1M
      * `make -C fw dfuprog`

  * Connect to the iCEBreaker-bitsy uart console (P0, P1) with a 1M baudrate
      * and then at the `Command>` prompt, press `r` for 'run'. This will
        start the USB detection and device should enumerate
