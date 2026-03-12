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
CPUID
## Phase 8
interupts and IDT (this also introduces the concept of different cpu privileges)
## Phase 9
Paging and page tables
## Phase 10
Segmentation and GDT

## Phase 11

FPU instructions

## Phase 12

SSE/AVX instructions


## Phase 13

AMX 

## Phase 14

LAPIC

## Phase 15
multiple virtual CPUs and IPI (but only hardcoded for wakeup)

## Phase 16

Real Mode quirks part 1

## Phase 17

Real Mode quirks part 2

## Phase 18

Protected Mode quirks part 1

## Phase 19

Protected Mode quirks part 2

## Phase 20
Long Mode quirks part 1

## Phase 21

Long Mode quirks part 2

## Phase 22

IO APIC


