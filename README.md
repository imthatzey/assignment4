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

The TLB hit count matches the sample `output.txt` (54 hits).

**Note for Part 3:** the page fault function should be `int handle_page_fault(int page, int *evicted_page)`. It returns the frame number and sets `*evicted_page` to the page it replaced, or -1 if it used an empty frame.

### Bakhtawar

<!-- To be added -->
