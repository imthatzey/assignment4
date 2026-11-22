/*
 * COMP SCI 3SH3 - Assignment 4
 *
 * PART 1: MMU - address translation.
 *   - read logical addresses from addresses.txt with fopen()
 *   - split each address into page number and offset with bitwise operators
 *   - TLB lookup -> page table lookup -> page fault, in that order
 *   - add the page to the TLB on every TLB miss
 *   - page table is an array initialised to -1 (demand paging)
 *
 * PART 2: TLB.
 *   - TLBentry struct (page, frame)
 *   - search_TLB, TLB_Add (FIFO), TLB_Update
 *   - TLB implemented as a circular array of 16 entries
 *
 * The page fault handler (Part 3) is only a placeholder below so the program
 * compiles and runs.
 *
 * Logical address space : 2^16 = 65,536 bytes  -> 16-bit addresses
 * Physical address space: 2^15 = 32,768 bytes  -> 15-bit addresses
 * Page / frame size     : 2^8  = 256 bytes     -> 8-bit offset
 *   Number of pages  = 2^16 / 2^8 = 256  (page number = 8 bits)
 *   Number of frames = 2^15 / 2^8 = 128  (frame number = 7 bits)
 */

#include <stdio.h>
#include <stdlib.h>

#define OFFSET_BITS  8
#define OFFSET_MASK  0xFF                 /* low 8 bits: 1111 1111 */
#define PAGE_SIZE    (1 << OFFSET_BITS)   /* 256 */
#define NUM_PAGES    256
#define NUM_FRAMES   128
#define TLB_SIZE     16

/* Page table: indexed by page number, holds the frame number, or -1 if the
 * page is not in memory. */
int page_table[NUM_PAGES];


/* =====================================================================
 * PART 2 - TLB
 * ===================================================================== */

/* One TLB entry: a page number and the frame it is in. page == -1 means the
 * slot is empty. */
typedef struct {
    int page;
    int frame;
} TLBentry;

/* The TLB is a circular array. tlb_next is the slot the next TLB_Add writes
 * to. Slots are filled 0, 1, ..., 15 and then reused in the same order, so
 * once the TLB is full, tlb_next always points at the oldest entry (FIFO). */
TLBentry tlb[TLB_SIZE];
int tlb_next = 0;

/* Empty every TLB slot. Called once at start-up. */
void init_TLB(void)
{
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].page = -1;
        tlb[i].frame = -1;
    }
    tlb_next = 0;
}

/* Search the TLB for an entry for `page`.
 * Returns the frame number on a TLB hit, or -1 on a TLB miss. */
int search_TLB(int page)
{
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].page == page)
            return tlb[i].frame;
    }
    return -1;
}

/* Add (page, frame) to the TLB. If the TLB is full, this overwrites the
 * oldest entry (FIFO). */
void TLB_Add(int page, int frame)
{
    tlb[tlb_next].page = page;
    tlb[tlb_next].frame = frame;
    tlb_next = (tlb_next + 1) % TLB_SIZE;
}

/* Called when `old_page` has been replaced in physical memory by `new_page`
 * (now in `new_frame`). If old_page has a TLB entry, the new page's entry is
 * written into that same slot (the simplification the handout allows).
 * If old_page is not in the TLB, fall back to TLB_Add. */
void TLB_Update(int old_page, int new_page, int new_frame)
{
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].page == old_page) {
            tlb[i].page = new_page;
            tlb[i].frame = new_frame;
            return;
        }
    }
    TLB_Add(new_page, new_frame);
}


/* =====================================================================
 * PART 3 PLACEHOLDER - replace with the real page fault handler.
 * ===================================================================== */

/* Just hands out frames 0, 1, 2, ... in order so there is a frame number to
 * work with. It does NOT read BACKING_STORE.bin and does NOT replace pages
 * properly when memory is full.
 *
 * What the real version must do (handout Part 3):
 *   - load `page` from BACKING_STORE.bin into a frame (FIFO replacement)
 *   - set page_table[page] = frame, and page_table[old page] = -1 if a page
 *     was replaced
 *   - store the replaced page number in *evicted_page, or -1 if a free frame
 *     was used (main uses this to call TLB_Update)
 *   - return the frame number */
static int placeholder_next_frame = 0;

int handle_page_fault(int page, int *evicted_page)
{
    int frame = placeholder_next_frame % NUM_FRAMES;
    placeholder_next_frame++;
    page_table[page] = frame;
    *evicted_page = -1;
    return frame;
}


/* =====================================================================
 * PART 1
 * ===================================================================== */

int main(void)
{
    /* Demand paging: no page is in memory at the start. */
    for (int i = 0; i < NUM_PAGES; i++)
        page_table[i] = -1;
    init_TLB();

    FILE *fptr = fopen("addresses.txt", "r");
    if (fptr == NULL) {
        perror("fopen addresses.txt");
        return 1;
    }

    int logical_address;
    int total = 0, page_faults = 0, tlb_hits = 0;

    while (fscanf(fptr, "%d", &logical_address) == 1) {
        total++;

        /* Split the address with bitwise operators. */
        int page   = (logical_address >> OFFSET_BITS) & 0xFF;  /* top 8 bits */
        int offset = logical_address & OFFSET_MASK;            /* low 8 bits */

        /* Check the TLB first. */
        int frame = search_TLB(page);

        if (frame != -1) {
            tlb_hits++;                          /* TLB hit */
        } else {
            /* TLB miss: check the page table. */
            frame = page_table[page];
            int evicted_page = -1;

            if (frame == -1) {                   /* page not in memory */
                page_faults++;
                frame = handle_page_fault(page, &evicted_page);
            }

            /* The frame was resolved via the page table (with or without a
             * page fault), so the page must now go into the TLB, once.
             *  - A page was replaced: TLB_Update puts the new page in the
             *    replaced page's slot (or falls back to TLB_Add).
             *  - Otherwise: TLB_Add. */
            if (evicted_page != -1)
                TLB_Update(evicted_page, page, frame);
            else
                TLB_Add(page, frame);
        }

        /* Physical address = frame bits followed by offset bits
         * (same as frame * PAGE_SIZE + offset). */
        int physical_address = (frame << OFFSET_BITS) | offset;

        /* Part 3 adds " Value=%d" (the byte read from physical memory). */
        printf("Virtual address: %d Physical address = %d\n",
               logical_address, physical_address);
    }

    fclose(fptr);

    printf("Total addresses = %d \n", total);
    printf("Page_faults = %d \n", page_faults);
    printf("TLB Hits = %d \n", tlb_hits);

    return 0;
}
