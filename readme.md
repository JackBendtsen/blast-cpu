# blast-cpu: A Custom 16-bit CPU Design

The Central Processing Unit is at the heart of every computer. Simply put, it's what makes everything tick.
It drives the software that operates it, and the hardware it connects to.

After some significant tinkering and learning of several different CPU architectures,
I developed a taste for them, through which I've decided on the things I like and dislike about most CPUs.
This inspired me to create my own!

Blast is a fully-featured 16-bit RISC CPU designed to run at 12MHz. It features:
* 24-bit addressing
* 8 16-bit general-purpose registers
* 1 20-bit Address register
* 1 20-bit Linking register (to store the return address from function calls)
* 1 Overflow register
* 1 Interrupt register

Efficiency and simplicity define this project, with many common features of a typical CPU being cut,
eg. Comparison registers, multiply & divide capabilities, etc.

The reason as to why I chose fairly archaic limitations for my design, such as 16-bit, low clock speed and limited-ish RAM are that
* it's easier to write an accurate emulator capable of running in real-time,
* that it would (hopefully) be extremely low-power,
* and that if I were determined enough to build a microprocessor out of my design,
   there's a chance that it would be doable. I like to keep my ambitions high!

----

## Content Breakdown

A list of each file and its significance & purpose.

* docs/ - Documentation for the Blast CPU
* assembler/ - A tool that lets you assemble and disassemble binaries for the Blast CPU
* emulator/ - A cycle-accurate emulator for the Blast CPU
* emulator/test-tool - A simple program designed to test the CPU
* tests/ - Test programs written in assembly for the Blast CPU