
#ifndef BUDDY

#include <stdlib.h>
#include <stdint.h>
#include "memory_manager.h"
#include <naiveGraphicsConsole.h>

#define MAX_SIZE 0x4000000 // 64Mb
#define START_MM 0x900000 // Memory Start Pool

//TODO ver esto
#define BYTE_ALIGMENT 8
#define ALIGMENT_MASK 0x0007

#define MINIMUM_BLOCK_SIZE (( size_t ) nodeSize << 1)
#define BITS_PER_BYTE (( size_t ) 8)

typedef struct freeNode 
{
    struct freeNode * next;
    size_t size;
} freeNode;

static freeNode startNode;
static freeNode * endNode = NULL;
static const size_t nodeSize = ( sizeof( freeNode ) + ( ( size_t ) ( BYTE_ALIGMENT - 1 ) ) ) & ~( ( size_t ) ALIGMENT_MASK );
static size_t freeBytesRemaining = 0U;
static size_t nodeAllocatedBit = 0;

static void addNodeToFreeList(freeNode * toAdd);

static void listInit() {
    freeNode * firstNode;
    uint8_t * alignedHeap;
    size_t auxAddress = (size_t) START_MM;
    size_t totalHeapSize = (size_t) MAX_SIZE;

    // TODO Ver si es necesario esto
    if ((auxAddress & ALIGMENT_MASK) != 0 ) {
        auxAddress += (BYTE_ALIGMENT - 1);
        auxAddress &= ~((size_t) ALIGMENT_MASK);
        totalHeapSize -= auxAddress - (size_t) START_MM;
    }

    alignedHeap = (uint8_t *) auxAddress;

    startNode.next = (void *) alignedHeap;
    startNode.size = 0U;

    auxAddress = ((size_t) alignedHeap) + totalHeapSize;
    auxAddress -= nodeSize;
    auxAddress &= ~((size_t) ALIGMENT_MASK);

    endNode = (void *) auxAddress;
    endNode->size = 0;  
    endNode->next = NULL;

    firstNode = (void *) alignedHeap;
    firstNode->size = auxAddress - (size_t) firstNode;
    firstNode->next = endNode;

    freeBytesRemaining = firstNode->size;

    nodeAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * BITS_PER_BYTE ) - 1 );
}

void * alloc(size_t wantedSize) {
    if (endNode == NULL)
        listInit();
    
    if ((wantedSize & nodeAllocatedBit) == 0) {
        if (wantedSize > 0 && ((wantedSize + nodeSize) > wantedSize)) {
            wantedSize += nodeSize;
            if ((wantedSize & ALIGMENT_MASK) != 0x00) {
                if (( wantedSize + ( BYTE_ALIGMENT - ( wantedSize & ALIGMENT_MASK ) ) ) > wantedSize)
                    wantedSize += BYTE_ALIGMENT - ( wantedSize & ALIGMENT_MASK );
                else
                    wantedSize = 0;
            }
        }
        else
            wantedSize = 0;
    }

    if (wantedSize == 0 || wantedSize > freeBytesRemaining)
        return NULL;

    freeNode * prevNode = &startNode;
    freeNode * iterator = startNode.next;
    void * toReturn = NULL;
    
    while ((iterator->size < wantedSize) && (iterator->next != NULL)) {
        prevNode = iterator;
        iterator = iterator->next;
    }
    if (iterator != endNode) {
        toReturn = (void *) (((uint8_t *) prevNode->next) + nodeSize);

        prevNode->next = iterator->next;
        
        if ((iterator->size - wantedSize) > MINIMUM_BLOCK_SIZE) {
            freeNode * newNode = (void *)((uint8_t *) iterator + wantedSize);

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

static void addNodeToFreeList(freeNode * toAdd) {
    freeNode * iterator = &startNode;
    uint8_t * puc;

    while (iterator->next < toAdd)
        iterator = iterator->next;
    
    puc = (uint8_t *) iterator;
    if ((puc + iterator->size) == ((uint8_t *) toAdd)) {
        iterator->size += toAdd->size;
        toAdd = iterator;
    }

    puc = (uint8_t *) toAdd;
    if ((puc + toAdd->size) == ((uint8_t *) iterator->next)) {
        if (iterator->next != endNode) {
            toAdd->size += iterator->next->size;
            toAdd->next = iterator->next->next;
        }
        else 
            toAdd->next = endNode;
    }
    else
        toAdd->next = iterator->next; 

    if (iterator != toAdd)
        iterator->next = toAdd;
}

void free(void * ptr) {
    if (ptr == NULL)
        return;
    uint8_t * puc = ((uint8_t *) ptr) - nodeSize;
    freeNode * node = (void *) puc;

    node->size &= ~(nodeAllocatedBit);

    freeBytesRemaining+= node->size;
    addNodeToFreeList((freeNode *)node);
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
   ngc_print("Bytes\n");
}

#endif