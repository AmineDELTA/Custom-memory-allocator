# Custom Memory Allocator in C

This project is a simple memory allocator built in C as part of a personal learning journey.  
It mimics how `malloc` and `free` work by using system calls like `mmap`, custom metadata, and a basic heap layout.

It's designed to help understand how memory is requested, managed, split, reused, and freed — all from scratch.

---

## 🧠 What It Does

- Allocates memory with a custom `my_malloc(size)`
- Frees memory with `my_free(ptr)`
- Uses `mmap()` to request memory from the OS
- Tracks memory blocks using a linked list and metadata
- Splits blocks when needed to avoid waste
- Merges adjacent free blocks to reduce fragmentation
- Aligns memory to 4 bytes

---

## 📚 Key Concepts Explored

- Manual memory management  
- Heap-like structure with metadata  
- Linked list traversal  
- Fragmentation and coalescing  
- Block splitting  
- Memory alignment using bitwise operations  

---

## 🔧 How It Works

Each memory block has metadata:

```c
typedef struct block_meta {
    size_t size;
    int free;
    struct block_meta* next;
    struct block_meta* prev;
} block_meta;
```

When memory is allocated, the allocator:

  - Looks for a free block big enough

  - If none is found, requests a new memory page from the OS

  - Splits large blocks if there's space to do so

  - Keeps track of everything in a linked list

---

💬 Notes

This project was built to learn and understand memory allocation — not to reinvent it.
It’s a hands-on way to see how low-level concepts work in practice.
It was a lot harder than expected, but worth every step.
