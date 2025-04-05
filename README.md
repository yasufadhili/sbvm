# Stack-Based Virtual Machine

A lightweight, portable stack-based virtual machine implementation in C.

## Overview

This project implements a simple stack-based virtual machine (VM) that executes bytecode instructions. The VM is designed with a clear separation between components, making it easy to understand, modify, and extend. It includes support for basic arithmetic operations, control flow, function calls, and memory access.

## Features

- **Stack-based architecture**: All operations work with a Last-In-First-Out (LIFO) stack
- **Rich instruction set**:
  - Basic stack operations (PUSH, POP, DUP, SWAP)
  - Arithmetic operations (ADD, SUB, MUL, DIV)
  - Control flow (JMP, JZ, JNZ)
  - Function calls (CALL, RET)
  - Memory access (LOAD, STORE)
  - System operations (PRINT, STOP)
- **32-bit integer operations**
- **Function call support** with a dedicated call stack
- **Memory segment** for variable storage

## Components

### OpCode

Defines the available instructions for the VM:

```c
typedef enum {
  OP_NOP,    // No operation
  OP_PUSH,   // Push value onto stack
  OP_POP,    // Pop value from stack
  OP_DUP,    // Duplicate top value
  OP_SWAP,   // Swap top two values
  // ... other operations
} OpCode;
```

### Stack

Manages the operand stack:

```c
typedef struct {
  int32_t data[STACK_MAX_SIZE];
  int32_t top;
} Stack;
```

### CallStack

Manages function calls and returns:

```c
typedef struct {
  CallFrame frames[CALL_STACK_MAX];
  int32_t count;
} CallStack;
```

### CodeSegment

Stores bytecode to be executed:

```c
typedef struct {
  uint8_t* code;    // Bytecode array
  size_t length;    // Length of bytecode
  size_t capacity;  // Allocated capacity
} CodeSegment;
```

### VM

Main virtual machine structure:

```c
typedef struct {
  CodeSegment* code_segment;
  Stack stack;
  CallStack call_stack;
  int32_t memory[MEMORY_SIZE];
  bool running;
} VM;
```

## Instruction Format

Instructions consist of an opcode byte followed by optional operands:

- `OP_PUSH`: Followed by a 4-byte (32-bit) integer value
- `OP_JMP`, `OP_JZ`, `OP_JNZ`, `OP_CALL`: Followed by a 4-byte target address
- Most other operations take their operands from the stack

## Usage Example

```c
#include <stdio.h>


int main() {
  // Initialise code segment
  CodeSegment code;
  code_segment_init(&code, 256);
  
  // Create a program that calculates (42 + 8)
  code_segment_write(&code, OP_PUSH);
  code_segment_write_int32(&code, 42);
  
  code_segment_write(&code, OP_PUSH);
  code_segment_write_int32(&code, 8);
  
  code_segment_write(&code, OP_ADD);
  code_segment_write(&code, OP_PRINT);
  code_segment_write(&code, OP_STOP);
  
  // Initialise and run the VM
  VM vm;
  vm_init(&vm, &code);
  vm_run(&vm);
  
  // Clean up
  code_segment_free(&code);
  
  return 0;
}
```

## Implementation Details

### Stack Operations

- `OP_PUSH`: Pushes a value onto the stack
- `OP_POP`: Removes and discards the top value
- `OP_DUP`: Duplicates the top value
- `OP_SWAP`: Swaps the top two values

### Arithmetic Operations

- `OP_ADD`: Pops two values, adds them, and pushes the result
- `OP_SUB`: Pops two values, subtracts the second from the first, and pushes the result
- `OP_MUL`: Pops two values, multiplies them, and pushes the result
- `OP_DIV`: Pops two values, divides the first by the second, and pushes the result

### Control Flow

- `OP_JMP`: Unconditional jump to a target address
- `OP_JZ`: Jump to a target address if the top value is zero
- `OP_JNZ`: Jump to a target address if the top value is not zero

### Function Calls

- `OP_CALL`: Call a function at a target address
- `OP_RET`: Return from a function call

### Memory Access

- `OP_LOAD`: Load a value from memory at the address specified by the top value
- `OP_STORE`: Store the second value on the stack to memory at the address specified by the top value

### System Operations

- `OP_PRINT`: Print the top value of the stack
- `OP_STOP`: Stop the VM execution

## Error Handling

The VM includes simple error handling for:
- Stack underflow and overflow
- Division by zero
- Invalid memory access
- Invalid jump targets
- Unknown opcodes

## Building and Running

```bash
make run
```

## Extending the VM

To add new instructions:
1. Add a new opcode to the `OpCode` enum
2. Implement the operation in the `vm_run` function's switch statement
3. Use the new opcode in the bytecode

## License

This project is available under the MIT License.