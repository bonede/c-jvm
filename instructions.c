#include "vm.h"
#include "class_file.h"

u2 get_u2(vm* vm, u4 offset){
    u1* code = vm->current_method->code.code;
    return *(code + vm->pc + offset + 1) | *(code + vm->pc + offset + 2);
}

int get_i2(vm* vm, u4 offset){
    u1* code = vm->current_method->code.code;
    return *(code + vm->pc + offset + 1) | *(code + vm->pc + offset + 2);
}

int get_i1(vm* vm, u4 offset){
    u1* code = vm->current_method->code.code;
    return *(code + vm->pc + offset + 1);
}

void aconst_null(vm* vm)
{
    push_int(vm, 0);
    vm->pc++;
}

void iconst_1(vm* vm)
{
    push_int(vm, 1);
    vm->pc++;
}

void iconst_2(vm* vm)
{
    push_int(vm, 2);
    vm->pc++;
}

void invokestatic(vm* vm)
{
    u2 method_ref_index = get_u2(vm, 0);
    methodref_info* method_ref = get_method_ref_info_at(vm->current_class, method_ref_index);
    
    if(method_ref->class_index != vm->current_class->this_class){
        panic("Method is not in this class", method_ref->class_index);
    }
    name_and_type_info* target_method_name_and_type = get_name_and_type_info_info_at(vm->current_class, method_ref->name_and_type_index);
    method_info* method = vm->current_class->methods;
    method_info* target_method = 0;
    for(int i = 0; i < vm->current_class->methods_count; i++){
        if(method->name_index == target_method_name_and_type->name_index &&
            method->descriptor_index == target_method_name_and_type->descriptor_index){
            target_method = method;
            break;
        }
        method += i;
    }
    if(!target_method){
        panic("Can't find method ref:", method_ref_index);
    }
    push_stack_frame(vm, vm->pc + 3, method);
}

void istore_1(vm* vm)
{
    // TODO implement
    set_int(vm, 1, pop_int(vm));
    vm->pc++;
}

void getstatic(vm *vm){
    // TODO implement
    vm->pc +=3;
}

void iload_1(vm* vm){
    push_int(vm, get_int(vm, 1));
    vm->pc++;
}

void iload_0(vm* vm){
    push_int(vm, get_int(vm, 0));
    vm->pc++;
}

void invokevirtual(vm* vm){
    
    u2 method_ref_index = get_u2(vm, 0);
    methodref_info* method_ref = get_method_ref_info_at(vm->current_class, method_ref_index);
    name_and_type_info* name_and_type = get_name_and_type_info_info_at(vm->current_class, method_ref->name_and_type_index);
    char class_name[256];
    char name[256];
    char descriptor[256];
    class_info* class_info = get_class_info_info_at(vm->current_class, method_ref->class_index);
    utf8_info_str(vm->current_class, class_info->name_index, class_name);
    utf8_info_str(vm->current_class, name_and_type->name_index, name);
    utf8_info_str(vm->current_class, name_and_type->descriptor_index, descriptor);
    invoke(vm, class_name, name, descriptor);
    // TODO implement
    vm->pc +=3;
}

void return_(vm* vm){
    if(*(vm->stack + vm->bp) == 0){
        vm->should_end = 1;
        return;
    }else{
        pop_stack_frame(vm);
    }
}

void iadd(vm* vm){
    push_int(vm, pop_int(vm) + pop_int(vm));
    vm->pc++;
}

void ireturn(vm* vm){
    int value = pop_int(vm);
    pop_stack_frame(vm);
    push_int(vm, value);
}

void sipush(vm* vm){
    int value = get_i2(vm, 0);
    push_int(vm, value);
    vm->pc += 3;
}

void bipush(vm* vm){
    int value = get_i1(vm, 0);
    push_int(vm, value);
    vm->pc += 2;
}