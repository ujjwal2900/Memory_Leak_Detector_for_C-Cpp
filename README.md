# Memory Leak Detector for C/C++

This project implements a custom garbage collector for C programs that detects memory leaks based on the reachability of objects. It provides a library to track object allocations and deallocations, and uses a graph traversal algorithm to identify unreachable objects, which are considered memory leaks.

## What This Project Does

The Memory Leak Detector (MLD) library allows developers to:
- Register C structures and their fields in a structure database
- Allocate and deallocate objects using custom `xcalloc` and `xfree` functions that track allocations
- Mark root objects (starting points for reachability analysis)
- Run a leak detection algorithm that traverses the object graph from root objects
- Report any objects that are not reachable from roots, indicating potential memory leaks

The core algorithm works by:
1. Maintaining a database of all allocated objects
2. Performing a depth-first search from root objects to mark reachable objects
3. Identifying unmarked objects as leaked memory

## Basic Skeleton Structure

The project consists of the following files:

- **`mld.h`**: Header file containing:
  - Data type definitions and enumerations
  - Structure database (`struct_db_t`) for tracking C structure metadata
  - Object database (`object_db_t`) for tracking allocated objects
  - Function declarations for MLD operations

- **`mld.c`**: Implementation file containing:
  - Structure database management functions
  - Object database management functions
  - Memory allocation/deallocation tracking (`xcalloc`, `xfree`)
  - Root object registration functions
  - The leak detection algorithm (`run_mld_algorithm`)
  - Reporting functions for leaked objects

- **`app.c`**: Example application demonstrating:
  - Structure registration using macros (`REG_STRUCT`, `FIELD_INFO`)
  - Object allocation and manipulation
  - Root object marking
  - Running the leak detection and reporting

- **`Makefile`**: Build configuration for compiling the project

## How It's Useful

This tool is particularly valuable for:
- **Debugging Memory Leaks**: Helps identify objects that are allocated but no longer referenced, preventing memory exhaustion
- **C Program Analysis**: Since C doesn't have built-in garbage collection, this provides a way to detect leaks manually
- **Educational Purposes**: Demonstrates garbage collection concepts and graph traversal algorithms
- **Custom Memory Management**: Can be integrated into larger C projects for memory tracking

The approach is similar to mark-and-sweep garbage collectors but focused specifically on leak detection rather than automatic cleanup.
