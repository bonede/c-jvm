#ifndef VM_FILE
#define VM_FILE

#include "types.h"
#include "class_file.h"

#define STACK_SIZE 1024 * 1024

#define STACK_HEADER 4
/*
    call stack frame 
    locals # fix length locals
    returningAddress   `0` for 1st frame
    returningMethodIndex `0` for 1st frame
    sp # last frame sp,  `0` for 1st frame
    bp # last frame bp, `0` for 1st frame
*/
typedef struct vm_
{
    method_info* current_method;
    u4 current_method_index;
    class_file* current_class;
    u1* heap;
    u4* stack;
    // pointer to current instruction
    u4 pc;
    // base of current stack frame
    u4 bp;
    u4 sp;
    // should end the vm
    u1 should_end;
} vm;

void push_int(vm* vm, int value);

int pop_int(vm* vm);

void set_int(vm* vm, int index, int value);

int get_int(vm* vm, int index);

void init_vm(vm* vm, u4* call_stack, u1* heap);

void run_vm(vm* vm, class_file* class_file, int argc, char** args);

void panic(char* msg, int code);

void push_stack_frame(vm* vm, u4 returning_address, method_info* target_method);

void pop_stack_frame(vm* vm);

u4* get_returning_address(vm* vm);

u4* get_returning_method_index(vm* vm);

void invoke(vm* vm, char* class_name, char* method_name, char* method_descriptor);

void print_int(int value);

#endif
