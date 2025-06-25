# Risc-Vm
My Implementation of a VM written in 32-bit RISC-V with compressed Instructions
I did not and will not (most likely) implement the Multiply, Atomic, and Floating Point Extentions that are included in the Simon Fraser University Reference Sheet.
### Build System
 * If you wish to build the vm all you have to do is download the zip [here](https://github.com/Gszopinski123/risc-vm/archive/refs/heads/main.zip) or you can use these following commands ``` wget https://github.com/Gszopinski123/risc-vm/archive/refs/heads/main.zip && unzip main.zip && rm -rf main.zip ```
 * once unzipped you should navigate to the top directory once there you may run ``` make ```, which will build the entire system and clear random executables and object files.
 * all you have to do is type ``` ./build/bin/riscv_32-vm <insert ELF file here> ``` and the vm will start to work.
 * if you wish to run the test files you may by running ``` bash run.sh <insert test#.elf> ```(caveat test9.elf does not work fully yet)


### References
I do not own any of these and are only using them.
 * I used this [Article](https://ayedaemon.medium.com/elf-chronicles-program-headers-433a7e4e97dd) for understanding ELF
 * I used this [reference](https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf) for getting the basics down for opcodes and other instructions.
 * I used this [resource](https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf) for a better understanding of riscv.
 * Also more resources are found in the docs.
