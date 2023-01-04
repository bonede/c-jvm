#include "vm.h"
#include "instructions.h"
#include "class_file.h"


void panic(char* msg, int code);

void debug(char* msg);

void print_int(int x);

u4* get_returning_address(vm* vm){
    return vm->stack + vm->bp + 3;
}

u4* get_returning_method_index(vm* vm){
    return vm->stack + vm->bp + 2;
}

static inline u4* get_last_sp(vm* vm){
    return vm->stack + vm->bp + 1;
}

static inline u4* get_last_bp(vm* vm){
    return vm->stack + vm->bp;
}

static inline u4* get_local_base(vm* vm){
    return vm->stack + vm->bp + STACK_HEADER;
}

void init_vm(vm* vm, u4* call_stack, u1* heap)
{
    vm->heap = heap;
    vm->pc = 0;
    vm->sp = 0;
    vm->bp = 0;
    vm->stack = call_stack;
    vm->should_end = 0;
    // zero out bp, returningAddress in first frame
    *get_last_bp(vm) = 0;
    *get_last_sp(vm) = 0;
    *get_returning_address(vm) = 0;
    *get_returning_method_index(vm) = 0;
}


void fetch_and_execute_instruction(vm* vm){
    code_attr method_code = vm->current_method->code;
    u1 opcode = *(method_code.code + vm->pc);
    switch (opcode)
    {
        case 4: iconst_1(vm); break;
        case 5: iconst_2(vm); break;
        case 184: invokestatic(vm); break;
        case 60: istore_1(vm); break;
        case 178: getstatic(vm); break;
        case 26: iload_0(vm); break;
        case 27: iload_1(vm); break;
        case 17: sipush(vm); break;
        case 16: bipush(vm); break;
        case 96: iadd(vm); break;
        case 182: invokevirtual(vm); break;
        case 172: ireturn(vm); break;
        case 177: return_(vm); break;
        default: panic("Unkown opcode ", opcode);
    }
}

void push_stack_frame(vm* vm, u4 returning_address, method_info* target_method){
    u4 new_bp = vm->bp + STACK_HEADER + vm->current_method->code.max_locals + vm->current_method->code.max_stack;
    // copy arguments from old operands stack to new locals
    for(int i = 0; i < target_method->args_counts; i++){
        *(vm->stack + new_bp + STACK_HEADER + target_method->args_counts - i - 1) = pop_int(vm);
    }
    *(vm->stack + new_bp) = vm->bp;
    *(vm->stack + new_bp + 1) = vm->sp;
    *(vm->stack + new_bp + 2) = vm->current_method - vm->current_class->methods;
    *(vm->stack + new_bp + 3) = returning_address;
    vm->bp = new_bp;
    vm->pc = 0;
    vm->sp = new_bp + STACK_HEADER + target_method->code.max_locals;
    vm->current_method = target_method;
}

void pop_stack_frame(vm* vm){
    // pop operands stack
    u4 last_bp = *get_last_bp(vm);
    u4 last_sp = *get_last_sp(vm);
    u4 returning_address = *get_returning_address(vm);
    u4 returning_method_index = *get_returning_method_index(vm);
    vm->bp = last_bp;
    vm->pc = returning_address;
    vm->sp = last_sp;
    vm->current_method = get_method_at(vm->current_class, returning_method_index);
}


void run_vm(vm* vm, class_file* class_file, int argc, char** args)
{
    if(*get_last_bp(vm) != 0 || *get_returning_address(vm) != 0){
        panic("Not initialized\n", 0);
    }
    method_info* main_method = get_main_method(class_file);
    if(!main_method){
        panic("No main method\n", 0);
    }
    
    vm->current_method = main_method;
    vm->current_class = class_file;
    vm->current_method_index =  main_method - vm->current_class->methods;
    vm->sp = vm->bp + STACK_HEADER + main_method->code.max_locals;
    for(;;){
        fetch_and_execute_instruction(vm);
        if(vm->should_end){
            break;
        }
    }
}

void set_int(vm* vm, int index, int value)
{
  *(get_local_base(vm) + index) = value;
}

int get_int(vm* vm, int index)
{
    return *(get_local_base(vm) + index);
}

void push_int(vm* vm, int value)
{
   
    *(vm->stack + vm->sp) = value;
    vm->sp++;
}

int pop_int(vm* vm)
{
    vm->sp--;
    return *(vm->stack + vm->sp);
}

void invoke(vm* vm, char* class_name, char* method_name, char* method_descriptor){
    if(
        strcmp("java/io/PrintStream", class_name) == 0 && 
        strcmp("print", method_name) == 0 &&
        strcmp("(I)V", method_descriptor) == 0
    )
    {
        print_int(pop_int(vm));
    }else{
        debug("Unimplemented method: ");
        debug(method_name);
        debug(method_descriptor);
        debug("\n");
        panic("Panic", 0);
    }
}

