# Roadmap

a phase is only considered completed when the instructions work in all supported modes (real, protected, long)

## Phase 1
simple ALU instrions 

ADD

SUB

MUL

DIV

MOD

OR

AND

NOT

SHR

SHL

for now those dont affect flags, but in the future they will
## Phase 2
unconditional jumps

JMP

## Phase 3

RFLAGS and ALU instructions that affect flags.

allready implemented instructions will be extended so that they will corectly set flags

## Phase 4
conditional jumps and conditional moves

## Phase 5
stack and memory operations/instructions

## Phase 6
function calls and returns

CALL

RET
## Phase 7

## Phase 8
CPUID
## Phase 9
interupts and IDT (this also introduces the concept of different cpu privileges)
## Phase 10
Paging and page tables
## Phase 11
Segmentation and GDT

## Phase 12

FPU instructions

## Phase 13

SSE/AVX instructions


## Phase 14

AMX 

## Phase 15

LAPIC

## Phase 16
multiple virtual CPUs and IPI (but only hardcoded for wakeup)

## Phase 17

Real Mode quirks part 1

## Phase 18

Real Mode quirks part 2

## Phase 19

Protected Mode quirks part 1

## Phase 20

Protected Mode quirks part 2

## Phase 21
Long Mode quirks part 1

## Phase 22

Long Mode quirks part 2

## Phase 23

IO APIC


