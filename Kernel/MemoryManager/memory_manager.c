#ifndef BUDDY
#define BUDDY

#include <stdlib.h>
#include <stdint.h>
#include "memory_manager.h"
#include <naiveGraphicsConsole.h>

#define MAX_SIZE 0x4000000 // 64Mb
#define START_MM 0x900000 // Memory Start Pool
#define BYTE_ALIGNMENT 8
#define ALIGNMENT_MASK (BYTE_ALIGNMENT - 1)
#define MINIMUM_BLOCK_SIZE (sizeof(freeNode) << 1)
#define BITS_PER_BYTE 8

typedef struct freeNode {
    struct freeNode *next;
    size_t size;
} freeNode;

static freeNode startNode;
static freeNode *endNode = NULL;
static const size_t nodeSize = (sizeof(freeNode) + ALIGNMENT_MASK) & ~ALIGNMENT_MASK;
static size_t freeBytesRemaining = 0U;
static size_t nodeAllocatedBit = 0;

static void addNodeToFreeList(freeNode *toAdd);

static void listInit() {
    freeNode *firstNode;
    uint8_t *alignedHeap;
    size_t auxAddress = (size_t)START_MM;
    size_t totalHeapSize = MAX_SIZE;

    // Align the start address
    if ((auxAddress & ALIGNMENT_MASK) != 0) {
        auxAddress += (BYTE_ALIGNMENT - 1);
        auxAddress &= ~ALIGNMENT_MASK;
        totalHeapSize -= auxAddress - START_MM;
    }

    alignedHeap = (uint8_t *)auxAddress;

    // Initialize start and end nodes
    startNode.next = (void *)alignedHeap;
    startNode.size = 0U;

    auxAddress = (size_t)alignedHeap + totalHeapSize;
    auxAddress -= nodeSize;
    auxAddress &= ~ALIGNMENT_MASK;

    endNode = (void *)auxAddress;
    endNode->size = 0;
    endNode->next = NULL;

    firstNode = (void *)alignedHeap;
    firstNode->size = auxAddress - (size_t)firstNode;
    firstNode->next = endNode;

    freeBytesRemaining = firstNode->size;

    nodeAllocatedBit = ((size_t)1) << ((sizeof(size_t) * BITS_PER_BYTE) - 1);
}

void *alloc(size_t wantedSize) {
    if (endNode == NULL) {
        listInit();
    }

    // Adjust the size for alignment and node overhead
    if (wantedSize > 0 && (wantedSize + nodeSize > wantedSize)) {
        wantedSize += nodeSize;
        if ((wantedSize & ALIGNMENT_MASK) != 0x00) {
            wantedSize += BYTE_ALIGNMENT - (wantedSize & ALIGNMENT_MASK);
        }
    } else {
        wantedSize = 0;
    }

    if (wantedSize == 0 || wantedSize > freeBytesRemaining) {
        return NULL;
    }

    freeNode *prevNode = &startNode;
    freeNode *iterator = startNode.next;
    void *toReturn = NULL;

    // Find the first fit block
    while (iterator->size < wantedSize && iterator->next != NULL) {
        prevNode = iterator;
        iterator = iterator->next;
    }

    if (iterator != endNode) {
        toReturn = (void *)((uint8_t *)iterator + nodeSize);
        prevNode->next = iterator->next;

        // Split the block if it is larger than needed
        if ((iterator->size - wantedSize) > MINIMUM_BLOCK_SIZE) {
            freeNode *newNode = (void *)((uint8_t *)iterator + wantedSize);
            newNode->size = iterator->size - wantedSize;
            iterator->size = wantedSize;
            addNodeToFreeList(newNode);
        }

        freeBytesRemaining -= iterator->size;
        iterator->size |= nodeAllocatedBit;
        iterator->next = NULL;
    }
    return toReturn;
}

static void addNodeToFreeList(freeNode *toAdd) {
    freeNode *iterator = &startNode;
    uint8_t *puc;

    while (iterator->next < toAdd) {
        iterator = iterator->next;
    }

    // Merge with previous block if adjacent
    puc = (uint8_t *)iterator;
    if ((puc + iterator->size) == (uint8_t *)toAdd) {
        iterator->size += toAdd->size;
        toAdd = iterator;
    }

    // Merge with next block if adjacent
    puc = (uint8_t *)toAdd;
    if ((puc + toAdd->size) == (uint8_t *)iterator->next) {
        if (iterator->next != endNode) {
            toAdd->size += iterator->next->size;
            toAdd->next = iterator->next->next;
        } else {
            toAdd->next = endNode;
        }
    } else {
        toAdd->next = iterator->next;
    }

    if (iterator != toAdd) {
        iterator->next = toAdd;
    }
}

void free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    uint8_t *puc = (uint8_t *)ptr - nodeSize;
    freeNode *node = (void *)puc;
    node->size &= ~nodeAllocatedBit;

    freeBytesRemaining += node->size;
    addNodeToFreeList(node);
}

void printMem() {
    ngc_print("Total Mem: 0x");
    ngc_printHex(MAX_SIZE);
    ngc_print(" Bytes\n");

    ngc_print("Used Mem: 0x");
    ngc_printHex(MAX_SIZE - freeBytesRemaining);
    ngc_print(" Bytes\n");

    ngc_print("Free Mem: 0x");
    ngc_printHex(freeBytesRemaining);
    ngc_print(" Bytes\n");
}

#endif // BUDDY
