#include "vm.h"
#include "vmlib.h"

// HELPER METHOD FOR FINDING THE BEST FIT
struct block_header *bestFit(struct block_header *heapstart, size_t size)
{

    // Traverse the entire heap to find the best-fitting block
    struct block_header *current_block = (struct block_header *)heapstart;
    struct block_header *best_fit_block = NULL;
    while (current_block->size_status != VM_ENDMARK) {
        if ((!(current_block->size_status & VM_BUSY)) &&
            BLKSZ(current_block) >= size) {

            // Case if the block is free and large enough
            // Update best_fit_blocks size
            if (BLKSZ(current_block) == size) {
                best_fit_block = current_block;
                best_fit_block->size_status = size;
                best_fit_block->size_status |= VM_PREVBUSY;
                // Set own status bit as busy
                best_fit_block->size_status |= VM_BUSY;
                break;

            } else if ((!best_fit_block) ||
                       (BLKSZ(current_block) < BLKSZ(best_fit_block))) {
                best_fit_block = current_block;
            }
        }

        // Calculate memory address of the next block header based on current
        // block's size
        current_block = (struct block_header *)((char *)current_block +
                                                (BLKSZ(current_block)));
    }

    // Case if there is not suitale block found
    return best_fit_block;
}

// HELPER METHOD FOR SPLITTING
struct block_header *splitBlock(struct block_header *split, size_t size)
{

    int prevBit = 0;

    // Calculate the remaining size of the memory after allocation is done
    size_t remaining_size = BLKSZ(split) - size;

    // Save prev size of split
    prevBit = split->size_status & VM_PREVBUSY;

    split->size_status = size;
    if (prevBit != 0) {
        split->size_status = split->size_status | VM_PREVBUSY;
    }

    split->size_status = split->size_status | VM_BUSY;

    // Check if the remaining size is enough for a new block
    if (remaining_size > sizeof(struct block_header)) {
        // Create the new free block header
        struct block_header *new_block =
            (struct block_header *)((char *)split + size);
        // Set size and clear status bit
        new_block->size_status = remaining_size;

        // Set the previous bit to 1 for the new block
        new_block->size_status |= prevBit; // VM_PREVBUSY set to 1

        if (new_block->size_status + BLKSZ(new_block) !=
            VM_ENDMARK) { // Check if not end mark
            // Update the previous bit of the next block if it's not the end
            // mark
            struct block_header *next_block =
                (struct block_header *)((char *)new_block) + (BLKSZ(new_block));
            next_block->size_status | 2; // Set the previous bit to 1
        }
        // Add a footer struct with new block pointing at the end, go backwards
        // by 8
        struct block_footer *footer =
            (struct block_footer *)((char *)new_block + remaining_size -
                                    sizeof(struct block_footer));
        footer->size = remaining_size; // Footer size points back to the header

        return split;
    }

    // If remaining size is not enough, do not split and just return the OG
    // block
    split->size_status |= 1; // Just set the status bit to 1 (allocated)
    return split;
}

void *vmalloc(size_t size)
{
    // TODO

    // Check to see if size is less than 0
    if (size <= 0) {
        return NULL;
    }

    // Calculate the required block size
    size_t block_size = size + sizeof(struct block_header);

    // Round up the block size to the nearest multiple of 16
    block_size = ROUND_UP(block_size, 16);

    // Call best_fit helper method
    struct block_header *best_fit_block = bestFit(heapstart, block_size);

    // Check if block was found
    if (best_fit_block == NULL) {
        return NULL;
    }

    // Calculate the remaining size of the memory after allocation is done
    size_t remaining_size = best_fit_block->size_status - block_size;

    // Split the block if needed
    if (remaining_size != 0) {
        splitBlock(best_fit_block, block_size);
    } else {
        // If no split happens, just mark the block as allocated
        best_fit_block->size_status |= 1; // Set the status bit to 1 (allocated)
    }

    // Return a pointer to the start of the next payload
    return (void *)((char *)best_fit_block + sizeof(struct block_header));
}
