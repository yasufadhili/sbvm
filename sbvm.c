#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


typedef enum {
  OP_NOP, // No operation
  OP_PUSH,
  OP_POP,
  OP_DUP, // Duplicate top
  OP_SWAP, // Swap top two values

  // Arithmetic
  OP_ADD,   
  OP_SUB,    
  OP_MUL,
  OP_DIV,

  // Control flow
  OP_JMP,    // Unconditional jump
  OP_JZ,     // Jump if zero
  OP_JNZ,    // Jump if not zero

  // Function calls
  OP_CALL,   // Call function
  OP_RET,    // Return from function

  // Memory access
  OP_LOAD,   // Load from memory
  OP_STORE,  // Store to memory

  // System
  OP_PRINT,
  OP_STOP,
} OpCode;


#define STACK_MAX_SIZE 1024

typedef struct {
  int32_t data[STACK_MAX_SIZE];
  int32_t top;
} Stack;

void stack_init(Stack *s){
  s->top = -1;
}

bool stack_isEmpty(Stack *s){
  return s->top == -1;
}

bool stack_isFull(Stack *s){
  return s->top >= STACK_MAX_SIZE -1;
}

void stack_push(Stack *s, int32_t val){
  if (stack_isFull(s)) {
    fprintf(stderr, "Stack overflow \n");
    exit(1);
  }
  s->data[++s->top] = val;
}

int32_t stack_pop(Stack* s){
  if (stack_isEmpty(s)) {
    fprintf(stderr, "Stack Underflow \n");
    exit(1);
  }
  return s->data[s->top--];
}

int32_t stack_peek(Stack *s){
  if (s->top < 0) {
    fprintf(stderr, "Empty Stack \n");
    exit(1);
  }
  return s->data[s->top];
}


typedef struct {
  size_t return_addr; // Return instruction pointer
  int32_t frame_pointer; // Base of current stack frame
} CallFrame;

// Manages function calls and return addresses
#define CALL_STACK_MAX 256
typedef struct {
  CallFrame frames[CALL_STACK_MAX];
  int32_t count; // Number of active frames
} CallStack ;


void call_stack_init(CallStack* cs){
  cs->count = 0;
}

void call_stack_push(CallStack* cs, size_t return_addr, int32_t frame_pointer){
  if (cs->count >= CALL_STACK_MAX) {
    fprintf(stderr, "Call stack overflow\n");
    exit(1);
  }
  cs->frames[cs->count].return_addr = return_addr;
  cs->frames[cs->count].frame_pointer = frame_pointer;
  cs->count++;
}

CallFrame call_stack_pop(CallStack* cs){
  if (cs->count <= 0) {
    fprintf(stderr, "Call stack underflow\n");
    exit(1);
  }
  return cs->frames[--cs->count];
}

// Stores byte code to be executed
typedef struct {
  uint8_t* code; // Bytecode array
  size_t length; // Length of bytecode
  size_t capacity; // Allocated capacity
} CodeSegment ;

void code_segment_init(CodeSegment* seg, size_t initial_capacity){
  seg->code = (uint8_t*) malloc(initial_capacity);
  seg->length = 0;
  seg->capacity = initial_capacity;
}

void code_segment_write(CodeSegment* seg, uint8_t byte){
  if (seg->length >= seg->capacity) {
    size_t new_capacity = seg->capacity * 2;
    seg->code = (uint8_t*)realloc(seg->code, new_capacity);
    seg->capacity = new_capacity;
  }
  seg->code[seg->length++] = byte;
}

typedef struct {
  CodeSegment* code_segment;
  Stack stack;
  bool running;
} VM ;

void vm_init(VM* vm, CodeSegment* code){
  vm->code_segment = code;
  stack_init(&vm->stack);
  vm->running = false;
}

void vm_push(VM* vm, int32_t val){
  stack_push(&vm->stack, val);
}

uint32_t vm_pop(VM* vm){
  return stack_pop(&vm->stack);
}

void vm_run(VM* vm){
  vm->running = true;
  size_t ip = 0; // Instruction Pointer

  while (vm->running && ip < vm->code_segment->length) {
    uint8_t opcode = vm->code_segment->code[ip++];
    
    switch (opcode) {
      case OP_NOP:
        break;
        
      case OP_PUSH: {
        // Read the value to push from the bytecode
        int32_t value = (int32_t)vm->code_segment->code[ip++];
        vm_push(vm, value);
        break;
      }
      
      case OP_POP:
        vm_pop(vm);
        break;
        
      case OP_DUP: {
        int32_t value = stack_peek(&vm->stack);
        vm_push(vm, value);
        break;
      }
      
      case OP_SWAP: {
        int32_t a = vm_pop(vm);
        int32_t b = vm_pop(vm);
        vm_push(vm, a);
        vm_push(vm, b);
        break;
      }
      
      // Arithmetic operations
      case OP_ADD: {
        int32_t b = vm_pop(vm);
        int32_t a = vm_pop(vm);
        vm_push(vm, a + b);
        break;
      }
      
      case OP_SUB: {
        int32_t b = vm_pop(vm);
        int32_t a = vm_pop(vm);
        vm_push(vm, a - b);
        break;
      }
      
      case OP_MUL: {
        int32_t b = vm_pop(vm);
        int32_t a = vm_pop(vm);
        vm_push(vm, a * b);
        break;
      }
      
      case OP_DIV: {
        int32_t b = vm_pop(vm);
        int32_t a = vm_pop(vm);
        if (b == 0) {
          fprintf(stderr, "Error: Division by zero\n");
          vm->running = false;
          break;
        }
        vm_push(vm, a / b);
        break;
      }
      
      case OP_PRINT:
        printf("%d\n", vm_pop(vm));
        break;
        
      case OP_STOP:
        vm->running = false;
        break;
        
      default:
        fprintf(stderr, "Unknown opcode: %d\n", opcode);
        vm->running = false;
        break;
    }
  }
}


int main(void) {
  CodeSegment code;
  code_segment_init(&code, 256);
  
  code_segment_write(&code, OP_PUSH);
  code_segment_write(&code, 5);

  code_segment_write(&code, OP_PUSH);
  code_segment_write(&code, 4);

  code_segment_write(&code, OP_ADD);
  code_segment_write(&code, OP_PRINT);
  
  /** 
  // Push 3 onto the stack
  code_segment_write(&code, OP_PUSH);
  code_segment_write(&code, 3);
  
  // Add them (5 + 3)
  code_segment_write(&code, OP_ADD);
  
  // Print the result
  code_segment_write(&code, OP_PRINT);
  
  // Push 10 onto the stack
  code_segment_write(&code, OP_PUSH);
  code_segment_write(&code, 10);
  
  // Push 2 onto the stack
  code_segment_write(&code, OP_PUSH);
  code_segment_write(&code, 2);
  
  // Divide them (10 / 2)
  code_segment_write(&code, OP_DIV);
  
  // Print the result
  code_segment_write(&code, OP_PRINT);
  
  // Duplicate the top value (should be 5)
  code_segment_write(&code, OP_DUP);
  
  // Multiply the two values on top (5 * 5)
  code_segment_write(&code, OP_MUL);
  
  // Print the result
  code_segment_write(&code, OP_PRINT);
  **/
  // Stop the VM
  code_segment_write(&code, OP_STOP);

  VM vm;
  vm_init(&vm, &code);
  vm_run(&vm);

  printf("Run finished\n");

  free(code.code);

  return 0;
}