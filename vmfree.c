#include "vm.h"
#include "vmlib.h"

/**
 * The vmfree() function frees the memory space pointed to by ptr,
 * which must have been returned by a previous call to vmmalloc().
 * Otherwise, or if free(ptr) has already been called before,
 * undefined behavior occurs.
 * If ptr is NULL, no operation is performed.
 */
void vmfree(void *ptr)
{
    // TODO

    // If there is a NULL pointer
    if (ptr == NULL) {
        return;
    }

    // Get the block header from the given pointer
    struct block_header *header =
        (struct block_header *)((char *)ptr - sizeof(struct block_header));

    // Check if the block is already free
    if ((header->size_status & 1) == 0) {
        return; // Since block is already free, do nothing
    }

    // Unset the status bit to mark the block as free
    header->size_status &= ~1;

    // Find the footer of the next block and update it
    struct block_footer *footer =
        (struct block_footer *)((char *)header + BLKSZ(header) -
                                sizeof(struct block_footer));
    footer->size = BLKSZ(header);

    // Unset the next block's previous bit
    struct block_header *next_block =
        (struct block_header *)((char *)header + +BLKSZ(header));

    if (next_block->size_status !=
        VM_ENDMARK) { // If the next block is not the end mark
        next_block->size_status &= ~2; // Unset the previous bit
    }

    // Coalesce with the previous block if it is also free
    // only do this when you are not at heapstart
    // NOT at heapstart and prev block is free
    if (((header->size_status & 2) == 0) && (header != heapstart)) {
        // Find the footer of the previous block
        struct block_footer *prev_footer =
            (struct block_footer *)((char *)header -
                                    sizeof(struct block_footer));
        struct block_header *prev_header =
            (struct block_header *)((char *)header - prev_footer->size);

        // Add the size of the current block to the previous block
        prev_header->size_status += BLKSZ(header);

        // Update the footer of the current block
        footer->size = BLKSZ(prev_header);

        // Update header to point to the previous block
        header = prev_header;
    }

    // Coalesce  with the next block if it is also free
    if ((next_block->size_status != VM_ENDMARK) &&
        (next_block->size_status & 1) == 0) {
        // Add the size of the next block to the current block
        header->size_status += BLKSZ(next_block);

        // Find the footer of the next block and update it
        struct block_footer *next_footer =
            (struct block_footer *)((char *)next_block + BLKSZ(next_block) -
                                    sizeof(struct block_footer));

        next_footer->size = BLKSZ(header);

        footer = next_footer;
    }
}
