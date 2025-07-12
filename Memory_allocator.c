#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>

#define PAGE_SIZE 4096 //one page is 4096 bytes
#define MIN_BLOCK_SIZE 32 //smallest split block size
#define ALIGN_TO_4(size) (((size) + 3) & ~3) //we need the address to end with 00

typedef struct block_meta { //Metadata for each memory block
    size_t size;
    int free;  //free block = 1, used block = 0
    struct block_meta* next; 
    struct block_meta* prev;
}block_meta;

#define META_SIZE sizeof(block_meta)

static block_meta* heap_head = NULL;

void* request_space(size_t size){
    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) return NULL;
    return ptr;
}

void split_block(block_meta* block, size_t size){
    if (block->size >= size + META_SIZE + MIN_BLOCK_SIZE) {
        block_meta* new_block = (block_meta*)((char*)block + META_SIZE + size);
        new_block->size = block->size - size - META_SIZE;
        new_block->free = 1;
        new_block->next = block->next;
        new_block->prev = block;
        if (new_block->next) new_block->next->prev = new_block;

        block->size = size;
        block->next = new_block;
    }
}

void coalesce_blocks(block_meta* block) { //this is quite confusing
    if (block->next && block->next->free){
        block->size += META_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next) block->next->prev = block;
    }
        if (block->prev && block->prev->free){
        block->prev->size += META_SIZE + block->size;
        block->prev->next = block->next;
        if (block->next) block->next->prev = block->prev;
    }
}

block_meta* find_free_block(size_t size){
    block_meta* curr = heap_head;
    while (curr) {
        if (curr->free && curr->size >= size)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void* my_malloc(size_t size){
    size = ALIGN_TO_4(size);
    if (size == 0) return NULL;

    block_meta* block = find_free_block(size);
    if (block){
    block->free = 0;          
    split_block(block, size); 
    return (void*)(block + 1); // (void*) for type safety
    }

    size_t total_size = size + META_SIZE;
    size_t alloc_size = ((total_size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE; //formula to find the memory we need to alloc

        block = (block_meta*)request_space(alloc_size);
    if (!block) return NULL;

    block->size = alloc_size - META_SIZE;
    block->free = 0;
    block->next = NULL;
    block->prev = NULL;

    if (!heap_head){
        heap_head = block;
    }else{
        block_meta* tail = heap_head;
        while (tail->next) tail = tail->next;
        tail->next = block;
        block->prev = tail;
    }
    split_block(block, size);
    return (void*)(block + 1);
}

void my_free(void* ptr){
    if (!ptr) return;
    block_meta* block = (block_meta*)ptr - 1;
    block->free = 1;
    coalesce_blocks(block);
}

void print_heap(){
    block_meta* curr = heap_head;
    int i = 0;
    while (curr){
        printf("Block %d: addr=%p, size=%zu, free=%d\n", i, (void*)curr, curr->size, curr->free);
        curr = curr->next;
        i++;
    }
    printf("-..----..-\n");
}

typedef struct{ // Example
    int x, y;
}Point;

int main(){
    void* a1 = my_malloc(100);
    void* a2 = my_malloc(200);
    my_free(a1);
    void* a3 = my_malloc(50);
    print_heap();

    my_free(a2);
    my_free(a3);
    print_heap();

    return 0;
}
