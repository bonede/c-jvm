#include "class_file.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>

#define buffer_size 1024 * 1024
#define constant_pool_size 256
#define interfaces_size 16
#define total_attributes 128
#define total_fields 128
#define total_methods 128
#define file_name "Main.class"
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

unsigned char buffer[buffer_size];
cp_info constant_pool[constant_pool_size];
attribute_info attributes[total_attributes];
field_info fields[total_fields];
method_info methods[total_methods];
u2 interfaces[interfaces_size];

u4 call_stack[1024 * 1204];
u4 operands_stack[1024 * 1204];
u1 heap[1024 * 1204];

void panic(char* msg, int code){
    printf("%s %d", msg, code);
    printf("\n");
    exit(0);
}
void debug(char* msg){
    printf("%s", msg);
}

void print_int(int value){
    printf("%d\n", value);
}

int main(int argc, char** args){
    FILE* file = fopen(file_name, "r");
    if (!file) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    // TODO handle read error
    fread(buffer, sizeof(*buffer), ARRAY_SIZE(buffer), file);
    class_file cls_file;
    int result = parse_class_file(buffer, &cls_file, constant_pool, interfaces, fields, methods, attributes);
    if(!result){
        fprintf(stderr, "invalid class file\n");
        return EXIT_FAILURE;
    }
    vm vm;
    init_vm(&vm, call_stack, heap);
    printf("[DEBUG] VM booted\n");
    run_vm(&vm, &cls_file, argc, args);
    printf("[DEBUG] VM finished\n");
}