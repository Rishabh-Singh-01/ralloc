# ralloc: Custom Memory Allocator Prototype

This project is a **custom memory allocator prototype** designed exclusively for **educational purposes** to illustrate fundamental memory management concepts in C.

**WARNING: This is a toy implementation and is not intended for use in any production-level or serious application.**

---

## Getting Started

To compile and run the demonstration program, follow these steps:

1.  **Clone the Repository:**
    ```bash
    git clone https://github.com/Rishabh-Singh-01/ralloc.git
    cd ralloc
    ```

2.  **Run the Demo:**
    The provided `Makefile` is configured to compile and execute the recommended demonstration file. Requires gcc.
    ```bash
    make run
    ```

3.  **Run the tests:**
    The provided `Makefile` is configured to compile and execute the recommended tests. Requires gcc.
    ```bash
    make test
    ```

4.  **Allocate Memory in your Code:**
    Once setup, use the below provided function to allocate memory.

    ```c
    void *ralloc(size_t size);
    ```

5.  **De-Allocate Memory in your Code:**
    Once setup and allocated memory, use the below provided function to de-allocate memory.

    ```c
    void ralloc_free(void *ptr);
    ```
---

## Codebase Structure

We provide two distinct versions of the allocator. For learning and understanding, **always refer to the Enhanced Demo file.**

### Recommended for Learning: `rallocDemoEnhanced.c`

This file contains the **more complete and refined implementation** suitable for educational demonstration.

* **File:** `core/rallocDemoEnhanced.c`
* **Note:** This enhanced version was refactored and edited with the assistance of an AI tool (ChatGPT) using the initial `core/ralloc.c` as a reference to improve clarity and structure.

### Historical Implementation: `ralloc.c` (Avoid)

This file is preserved purely as a historical record of the initial development attempt.

* **File:** `core/ralloc.c`
* **Warning:** This represents a "first try" implementation written without significant refactoring. **It should not be used as a reference or source of inspiration for best practices.**

---

## Known Limitations

As a non-production prototype, both versions have significant functional limitations:

### `core/rallocDemoEnhanced.c` Limitations

* **Edge Cases:** The implementation does not fully account for various complex allocation/deallocation edge cases.
* **Testing:** The code has not been thoroughly validated or extensively tested, including against the current set of written test cases.

### `core/ralloc.c` Limitations

* **Metadata Management:** Metadata for the initial large free chunk is not populated by default; it is created only upon the first allocation request.
* **Fragmentation:** There is no logic implemented for fragmentation correction or coalescing adjacent free blocks.
* **Reliability:** Test execution can be inconsistent, suggesting potential issues in implementation logic or flaws within the test structure itself.
