#ifndef CLASS_FILE
#define CLASS_FILE

#include "types.h"

#define CONST_ITEM_SIZE 256
#define ATTRIBUTE_ITEM_SIZE 1024

#define MAGIC 0xcafebabe

#define CONSTANT_Class	7
#define CONSTANT_Fieldref	9
#define CONSTANT_Methodref	10
#define CONSTANT_InterfaceMethodref	11
#define CONSTANT_String	8
#define CONSTANT_Integer	3
#define CONSTANT_Float	4
#define CONSTANT_Long	5
#define CONSTANT_Double	6
#define CONSTANT_NameAndType	12
#define CONSTANT_Utf8	1
#define CONSTANT_MethodHandle	15
#define CONSTANT_MethodType	16
#define CONSTANT_Dynamic	17
#define CONSTANT_InvokeDynamic	18
#define CONSTANT_Module	19
#define CONSTANT_Package	20

#define ACC_PUBLIC	0x0001
#define ACC_PRIVATE	0x0002
#define ACC_PROTECTED 0x0004
#define ACC_STATIC	0x0008
#define ACC_FINAL 0x0010
#define ACC_SUPER 0x0020
#define ACC_INTERFACE 0x0200
#define ACC_ABSTRACT 0x0400
#define ACC_SYNTHETIC 0x1000
#define ACC_ANNOTATION 0x2000
#define ACC_ENUM	0x4000
#define ACC_VOLATILE	0x0040
#define ACC_TRANSIENT	0x0080
#define ACC_SYNTHETIC	0x1000

#define MAIN_METHOD_NAME "main"
#define MAIN_METHOD_DESCRIPTOR "([Ljava/lang/String;)V"
#define CODE_ATTR_NAME "Code"

int strcmp (const char *p1, const char *p2);

typedef struct cp_info_ {
    u1 tag;
    u1 info[CONST_ITEM_SIZE];
} cp_info;

typedef struct class_info_ {
    u2 name_index;
} class_info;

typedef struct fieldref_info_ {
    u2 class_index;
    u2 name_and_type_index;
} fieldref_info;

typedef struct methodref_info_ {
    u2 class_index;
    u2 name_and_type_index;
}  methodref_info;

typedef struct interface_methodref_info_ {
    u2 class_index;
    u2 name_and_type_index;
} interface_methodref;

typedef struct string_info_ {
    u2 string_index;
} string_info;

typedef struct integer_info_ {
    int value;
} integer_info;

typedef struct float_info_ {
    float value;
} float_info;

typedef struct long_info_ {
    long value;
} long_info;

typedef struct double_info_ {
    double value;
} double_info;

typedef struct name_and_type_info_ {
    u2 name_index;
    u2 descriptor_index;
} name_and_type_info;

typedef struct utf8_info_ {
    u2 length;
    u1 bytes[CONST_ITEM_SIZE - 2];
} utf8_info;

typedef struct method_handle_info_ {
    u1 reference_kind;
    u2 reference_index;
} method_handle_info;

typedef struct method_type_info_ {
    u2 descriptor_index;
} method_type_info;

typedef struct invoke_dynamic_info_ {
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;
} invoke_dynamic_info;


typedef struct attribute_info_
{
    u2 attribute_name_index;
    u4 attribute_length;
    u1 info[ATTRIBUTE_ITEM_SIZE];
} attribute_info;

typedef struct field_info_
{
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    attribute_info* attributes;
} field_info;

typedef struct code_attr_ {
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1* code;
    // TODO parse more fields https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.3
} code_attr;

typedef struct method_info_ {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    u2 args_counts;
    code_attr code;
    attribute_info* attributes;
} method_info;


typedef struct class_file_
{
    u4 magic;
    u2 minor_version;
    u1 major_version;
    u2 constant_pool_count;
    cp_info* constant_pool;
    u2 access_flags;
    u2 this_class;
    u2 super_class;
    u2 interfaces_count;
    u2* interfaces;
    u2 fields_count;
    field_info* fields;
    u2 methods_count;
    method_info* methods;
    u2 attributes_count;
    attribute_info* attributes;
} class_file;



int parse_class_file(
    u1* class_file_bytes, 
    class_file* class_file, 
    cp_info* constant_pool, 
    u2* interfaces,
    field_info* fields,
    method_info* methods,
    attribute_info* attributes
);

utf8_info* get_utf8_info_at(class_file* class_file, int index);

methodref_info* get_method_ref_info_at(class_file* class_file, int index);

class_info* get_class_info_info_at(class_file* class_file, int index);

name_and_type_info* get_name_and_type_info_info_at(class_file* class_file, int index);

void utf8_info_str(class_file* class_file, int index, char* buffer);

void utf8_to_str(utf8_info* utf8, char* buffer);

method_info* get_main_method(class_file* class_file);

method_info* get_method_at(class_file* class_file, u4 index);

void panic(char* msg, int code);


#endif