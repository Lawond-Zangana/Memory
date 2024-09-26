- This project involves the development of a custom memory allocation system in C. The primary goal is to create a dynamically linked library that includes custom implementations of the `malloc` and `free` functions, facilitating memory management at a low level. It emphasizes key systems programming concepts such as bitwise operations, pointer arithmetic, and efficient memory handling.
    
    The codebase consists of multiple files, including a public library interface (`vmlib.h`), internal data structures for memory management (`vm.h`), and the core implementation files for heap initialization, memory allocation (`vmalloc`), and memory deallocation (`vmfree`). Testing and debugging are supported through a test file (`vmtest.c`) and helper utilities.
    
    The project simulates a heap within a large block of memory, utilizing headers, footers, and block metadata for managing allocations. The resulting library can be linked with other programs, making it a foundational project for learning about memory allocation and systems programming in C.
