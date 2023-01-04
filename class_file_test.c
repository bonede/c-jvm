#include "class_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define buffer_size 1024 * 1024
#define constant_pool_size 256
#define interfaces_size 16
#define total_attributes 128
#define total_fields 128
#define total_methods 128
#define file_name "Main.class"

unsigned char buffer[buffer_size];
cp_info constant_pool[constant_pool_size];
attribute_info attributes[total_attributes];
field_info fields[total_fields];
method_info methods[total_methods];
u2 interfaces[interfaces_size];

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void print_cp_info(int i, cp_info* cp_info){
    utf8_info* utf8;
    char buffer[256];
    switch (cp_info->tag)
    {
    case CONSTANT_Utf8:
        utf8 = (utf8_info *) cp_info->info;
        // TODO handle overflow
        utf8_to_str(utf8, buffer);
        printf("const_poll[%d] %d %s\n", i, cp_info->tag, buffer);
        break;
    
    default:
        printf("const_poll[%d] %d\n", i, cp_info->tag);
        break;
    }   
}

void print_attribute(class_file* class_file, attribute_info* attr){
    char name_buffer[256];
    printf("  attr: ");
    utf8_info_str(class_file, attr->attribute_name_index, name_buffer);
    printf("%s bytes[%d]\n", name_buffer, attr->attribute_length);
}

void print_filed_info(class_file* class_file, int i, field_info* field_info){
    char name_buffer[256];
    char descriptor_buffer[256];
    utf8_info_str(class_file, field_info->name_index, name_buffer);
    utf8_info_str(class_file, field_info->descriptor_index, descriptor_buffer);
    printf("field[%d] flags = 0x%04x attrs<%d> %s %s\n", 
    i, field_info->access_flags, field_info->attributes_count, name_buffer, descriptor_buffer);
    int j = 0;
    for(j = 0; j < field_info->attributes_count; j++){
        print_attribute(class_file, field_info->attributes++);
    }
}

void print_method_info(class_file* class_file, int i, method_info* method){

    char name_buffer[256];
    char descriptor_buffer[256];
    utf8_info_str(class_file, method->name_index, name_buffer);
    utf8_info_str(class_file, method->descriptor_index, descriptor_buffer);
    printf("method[%d] flags = 0x%04x attrs<%d> args=%d %s%s\n", 
        i, method->access_flags, method->attributes_count, method->args_counts, name_buffer, descriptor_buffer);
    int j;
    for(j = 0; j < method->attributes_count; j++){
        print_attribute(class_file, method->attributes++);
    }
}

void panic(char* msg, int code){
    printf("%s %d", msg, code);
    printf("\n");
    exit(1);
}

int main(){
    int i;
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
    printf("magic %x\n", cls_file.magic);
    printf("minor_version = %d\n", cls_file.minor_version);
    printf("major_version = %d\n", cls_file.major_version);
    printf("constant_pool_count = %d\n", cls_file.constant_pool_count);
    printf("access_flags = 0x%04x\n", cls_file.access_flags);
    printf("this_class = %d\n", cls_file.this_class);
    printf("super_class = %d\n", cls_file.super_class);
    printf("interfaces_count = %d\n", cls_file.interfaces_count);
    printf("fields_count = %d\n", cls_file.fields_count);
    printf("method_count = %d\n", cls_file.methods_count);
    printf("attributes_count = %d\n", cls_file.attributes_count);

    for(i = 0; i < cls_file.constant_pool_count - 1; i++){
        print_cp_info(i, &constant_pool[i]);
    }
    for(i = 0; i < cls_file.fields_count; i++){
        print_filed_info(&cls_file, i, &fields[i]);
    }
    for(i = 0; i < cls_file.methods_count; i++){
        print_method_info(&cls_file, i, cls_file.methods + i);
    }
    printf("class attrs: \n");
    for(i = 0; i < cls_file.attributes_count; i++){
        print_attribute(&cls_file, cls_file.attributes + i);
    }
    method_info* main_method = get_main_method(&cls_file);
    if(main_method){
        printf("has main method\n");
    }else{
        printf("no main method\n");
    }
    
    return 0;
}

