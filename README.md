# COMP SCI 3SH3 – Assignment 4 (Group 64)

## How to compile and run

Run from the folder that contains `addresses.txt` and `BACKING_STORE.bin`:

```
gcc -o assignment4 Assignment4.c
./assignment4
```

## Group members and contributions

### Zeynep

**Part 1: Address translation**

- Read the addresses from `addresses.txt`.
- Split each address into a page number and an offset using bitwise operators.
- Looked up each page in the TLB first, then in the page table. If the page is in neither, it's a page fault.
- Made the page table an array that starts with every entry set to -1.
- Calculated and printed the physical address, and counted the page faults and TLB hits.

**Part 2: TLB**

- Made the TLB a circular array of 16 `TLBentry` items (page and frame pairs).
- Wrote `search_TLB` to find a page in the TLB.
- Wrote `TLB_Add` to add a page. When the TLB is full, it replaces the oldest entry (FIFO).
- Wrote `TLB_Update` to put a new page in the slot of a page that was removed from memory.

### Bakhtawar

**Part 3: Page faults**

- Opened `BACKING_STORE.bin` with `open()` and mapped it into memory with `mmap()`.
- Made physical memory a `signed char` array of 128 frames × 256 bytes.
- On a page fault, copied the 256-byte page from the backing store into a frame with `memcpy()`.
- Used frames in order (0 to 127, then back to 0), so when memory is full the oldest page is replaced (FIFO).
- When a page is replaced, set its page table entry back to -1 and updated the TLB.
- Printed the signed byte value stored at each physical address.

The program's output matches the sample `output.txt` exactly (538 page faults, 54 TLB hits).
