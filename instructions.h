#ifndef INSTRUCTIONS_FILE
#define INSTRUCTIONS_FILE
#include "vm.h"


void panic(char* msg, int code);

void aconst_null(vm* vm);
void iconst_1(vm* vm);
void iconst_2(vm* vm);
void invokestatic(vm* vm);
void istore_1(vm* vm);
void getstatic(vm* vm);
void iload_1(vm* vm);
void invokevirtual(vm* vm);
void return_(vm* vm);
void iload_0(vm* vm);
void iadd(vm* vm);
void ireturn(vm* vm);
void sipush(vm* vm);
void bipush(vm* vm);
#endif