# Current Obstacles to desktop ports
Right now, the code should build and run correctly on 64-bit, little-endian *nix platforms.

Some data size issues will probably come up on 32-bit platforms, but these will be solved later.

On big-endian platforms the `'` operator (address of variable) is currently incorrect for variables smaller than the default cell.
This will be corrected in the future with per-size custom operators.

Ports to microcontrollers will be explored once the language core has stabilized.