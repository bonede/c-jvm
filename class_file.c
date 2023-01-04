#include "class_file.h"
#include "byteorder.h"
#include "types.h"

/*

ClassFile {
    u4             magic;
    u2             minor_version;
    u2             major_version;
    u2             constant_pool_count;
    cp_info        constant_pool[constant_pool_count-1];
    u2             access_flags;
    u2             this_class;
    u2             super_class;
    u2             interfaces_count;
    u2             interfaces[interfaces_count];
    u2             fields_count;
    field_info     fields[fields_count];
    u2             methods_count;
    method_info    methods[methods_count];
    u2             attributes_count;
    attribute_info attributes[attributes_count];
}

*/

double pow(double x, double y);

u4 read_u4(u1** bytes)
{
    u4 value = U4(**((unsigned int **) bytes));
    *bytes += 4;
    return value;
}

int read_int(u1** bytes)
{
    u4 value = read_u4(bytes);
    return *(int *)&value;
}

float read_float(u1** bytes)
{
    u4 value = read_u4(bytes);
    return *(float *)&value;
}

long read_long(u1** bytes)
{
    u4 high_bytes = read_u4(bytes);
    u4 low_bytes = read_u4(bytes);
    return ((long) high_bytes << 32) + low_bytes;
}

utf8_info* get_utf8_info_at(class_file* class_file, int index){
    return (utf8_info*) (class_file->constant_pool + index - 1)->info;
}

methodref_info* get_method_ref_info_at(class_file* class_file, int index){
    return (methodref_info*) (class_file->constant_pool + index - 1)->info;
}

name_and_type_info* get_name_and_type_info_info_at(class_file* class_file, int index){
    return (name_and_type_info*) (class_file->constant_pool + index - 1)->info;
}

class_info* get_class_info_info_at(class_file* class_file, int index){
    return (class_info*) (class_file->constant_pool + index - 1)->info;
}

void utf8_info_str(class_file* class_file, int index, char* buffer){
    utf8_info* utf8 = get_utf8_info_at(class_file, index);
    utf8_to_str(utf8, buffer);
}

void utf8_to_str(utf8_info* utf8, char* buffer){
    for(int i = 0; i < utf8->length; i++){
        *buffer++ = utf8->bytes[i];
    }
    *buffer = 0;
}


double read_double(u1** bytes)
{
    u4 high_bytes = read_u4(bytes);
    u4 low_bytes = read_u4(bytes);
    long bits = ((long) high_bytes << 32) + low_bytes;
    int s = ((bits >> 63) == 0) ? 1 : -1;
    int e = (int)((bits >> 52) & 0x7ffL);
    long m = (e == 0) ?
            (bits & 0xfffffffffffffL) << 1 :
            (bits & 0xfffffffffffffL) | 0x10000000000000L;
    return pow(2, e-1075) * s * m;
}


u2 read_u2(u1** bytes)
{
    u2 value = U2(**((unsigned short**) bytes));
    *bytes += 2;
    return value;
}

u1 read_u1(u1** bytes)
{
    u1 value = **((unsigned char**) bytes);
    *bytes += 1;
    return value;
}

void read_bytes(u1** bytes, u1* target, u4 length)
{
    // TODO handle overflow
    for(int i = 0; i < length; i++){
        *target = **bytes;
        *bytes += 1;
        target += 1;
    }
}

void read_utf8(u1** bytes, u1* target, u4 length)
{
    // TODO handle overflow and utf8 stream
    for(int i = 0; i < length; i++){
        *target = **bytes;
        *bytes += 1;
        target += 1;
    }
}

void parse_cp_info(u1** class_file_bytes, cp_info* cp_info)
{
    u1 tag = read_u1(class_file_bytes);
    u2 length;
    cp_info->tag = tag;
    switch (tag)
    {
        case CONSTANT_Class:
            ((class_info *) cp_info->info)->name_index = read_u2(class_file_bytes);
            break;
        case CONSTANT_Fieldref:
            ((fieldref_info *) cp_info->info)->class_index = read_u2(class_file_bytes);
            ((fieldref_info *) cp_info->info)->name_and_type_index = read_u2(class_file_bytes);
            break;
        case CONSTANT_Methodref:
            ((methodref_info *) cp_info->info)->class_index = read_u2(class_file_bytes);
            ((methodref_info *) cp_info->info)->name_and_type_index = read_u2(class_file_bytes);
            break;
        case CONSTANT_InterfaceMethodref:
            ((interface_methodref *) cp_info->info)->class_index = read_u2(class_file_bytes);
            ((interface_methodref *) cp_info->info)->name_and_type_index = read_u2(class_file_bytes);
            break;
        case CONSTANT_String:
            ((string_info *) cp_info->info)->string_index = read_u2(class_file_bytes);
            break;
        case CONSTANT_Integer:
            ((integer_info *) cp_info->info)->value = read_int(class_file_bytes);
            break;
        case CONSTANT_Float:
            ((float_info *) cp_info->info)->value = read_float(class_file_bytes);
            break;
        case CONSTANT_Long:
            ((long_info *) cp_info->info)->value = read_long(class_file_bytes);
             break;
        case CONSTANT_Double:
            ((double_info *) cp_info->info)->value = read_double(class_file_bytes);
            break;
        case CONSTANT_NameAndType:
            ((name_and_type_info *) cp_info->info)->name_index = read_u2(class_file_bytes);
            ((name_and_type_info *) cp_info->info)->descriptor_index = read_u2(class_file_bytes);
            break;
        case CONSTANT_Utf8:
            length = read_u2(class_file_bytes);
            ((utf8_info *) cp_info->info)->length = length;
            read_utf8(class_file_bytes, ((utf8_info *) cp_info->info)->bytes, length);
            break;
        case CONSTANT_MethodHandle:
            ((method_handle_info *) cp_info->info)->reference_kind = read_u1(class_file_bytes);
            ((method_handle_info *) cp_info->info)->reference_index = read_u2(class_file_bytes);
            break;
        case CONSTANT_InvokeDynamic:
            ((invoke_dynamic_info *) cp_info->info)->bootstrap_method_attr_index = read_u2(class_file_bytes);
            ((invoke_dynamic_info *) cp_info->info)->name_and_type_index = read_u2(class_file_bytes);
            break;
        default:
            break;
    }
}

void parse_attribute(u1** class_file_bytes, attribute_info* attribute_info)
{
    attribute_info->attribute_name_index = read_u2(class_file_bytes);
    attribute_info->attribute_length = read_u4(class_file_bytes);
    // handle overflow
    read_bytes(class_file_bytes, attribute_info->info, attribute_info->attribute_length);
}

void parse_field(u1** class_file_bytes, field_info* field, attribute_info** attribute_info){
    field->access_flags = read_u2(class_file_bytes);
    field->name_index = read_u2(class_file_bytes);
    field->descriptor_index = read_u2(class_file_bytes);
    field->attributes_count = read_u2(class_file_bytes);
    field->attributes = *attribute_info;
    // TODO handle overflow
    for(int i = 0; i < field->attributes_count; i++){
        parse_attribute(class_file_bytes, (*attribute_info)++);
    }
}

u2 parse_args_count(class_file* class_file, u2 descriptor_index)
{
    char buffer[256];
    u2 args_count = 0;
    utf8_info_str(class_file, descriptor_index, buffer);
    u1 is_array = 0;
    u1 is_class = 0;
    for(int i = 1; i < 256; i++){ // skip '('
        char c = buffer[i];
        u1 is_1 = c == 'B' || c == 'C' || c == 'D'|| 
            c == 'F'|| c == 'I'|| c == 'S'|| c == 'Z';
        u1 is_2 = c == 'D' || c == 'J';
        if(c == ')'){
            break;
        }else if(is_1 && !is_class){
            args_count += 1;
            is_array = 0;
        }else if(is_2 && !is_class){
            if(is_array){
                args_count += 1;
            }else{
                args_count += 2;
            }
            is_array = 0;
        }else if(c == '['){
            is_array = 1;
        }else if(c == 'L'){
            is_class = 1;
        }else if(c == ';'){
            args_count += 1;
            is_class = 0;
            is_array = 0;
        }else if(is_class){
            continue;
        }else{
            panic("Dose not support argument type", c);
        }
    }
    return args_count;
}

void parse_method(class_file* class_file, u1** class_file_bytes, method_info* method, attribute_info** attribute_info){
    method->access_flags = read_u2(class_file_bytes);
    method->name_index = read_u2(class_file_bytes);
    method->descriptor_index = read_u2(class_file_bytes);
    method->attributes_count = read_u2(class_file_bytes);
    method->attributes = *attribute_info;
    method->args_counts = parse_args_count(class_file, method->descriptor_index);
    // TODO handle overflow
    char attr_name[256];
    u1* code_bytes;
    for(int i = 0; i < method->attributes_count; i++){
        parse_attribute(class_file_bytes, (*attribute_info)++);
        utf8_info_str(class_file, (method->attributes + i)->attribute_name_index, attr_name);
        if(strcmp(attr_name, CODE_ATTR_NAME) == 0){
            code_bytes = (method->attributes + i)->info;
            method->code.max_stack = read_u2(&code_bytes);
            method->code.max_locals = read_u2(&code_bytes);
            method->code.code_length = read_u4(&code_bytes);
            method->code.code = code_bytes;
        }
    }
}

int parse_class_file(
    u1* class_file_bytes, 
    class_file* class_file, 
    cp_info* constant_pool, 
    u2* interfaces,
    field_info* fields,
    method_info* methods,
    attribute_info* attributes
)
{
    int i;
    u4 magic = read_u4(&class_file_bytes);
    if(MAGIC !=  magic){
        return 0;
    }
    class_file->constant_pool = constant_pool;
    class_file->interfaces = interfaces;
    class_file->magic = magic;   
    class_file->interfaces = interfaces;
    class_file->fields = fields;
    class_file->methods = methods;
    class_file->minor_version = read_u2(&class_file_bytes);
    class_file->major_version = read_u2(&class_file_bytes);
    class_file->constant_pool_count = read_u2(&class_file_bytes);
    for(i = 0 ; i < class_file->constant_pool_count - 1; i++){
        // TODO handle overflow constant_pool
        parse_cp_info(&class_file_bytes, constant_pool++);
    }
    class_file->access_flags = read_u2(&class_file_bytes);
    class_file->this_class = read_u2(&class_file_bytes);
    class_file->super_class = read_u2(&class_file_bytes);

    class_file->interfaces_count = read_u2(&class_file_bytes);
    for(i = 0; i < class_file->interfaces_count; i++){
        // TODO handle overflow interface
        *(class_file->interfaces + i) = read_u2(&class_file_bytes);
    }

    class_file->fields_count = read_u2(&class_file_bytes);
    for(i = 0; i < class_file-> fields_count; i++){
        // TODO handle overflow fields
        parse_field(&class_file_bytes, fields + i, &attributes);
    }

    class_file->methods_count = read_u2(&class_file_bytes);
    for(i = 0; i < class_file->methods_count; i++){
        // TODO handle overflow methods
        parse_method(class_file, &class_file_bytes, methods + i, &attributes);
    }
    class_file->attributes_count = read_u2(&class_file_bytes);
    class_file->attributes = attributes;
    for(i = 0; i < class_file->attributes_count; i++){
        // TODO handle overflow attributes 
        parse_attribute(&class_file_bytes, attributes++);
    }
    return 1;
}

method_info* get_main_method(class_file* class_file){
    // TODO handle overflow
    char name[256];
    char descriptor[256];
    method_info* method;
    for(int i = 0; i < class_file->methods_count; i++){
        method = class_file->methods + i;
        utf8_info_str(class_file, method->name_index, name);
        utf8_info_str(class_file, method->descriptor_index, descriptor);
        if(strcmp(name, MAIN_METHOD_NAME) == 0 && strcmp(descriptor, MAIN_METHOD_DESCRIPTOR) == 0){
            return method;
        }
    }
    return 0;
}

method_info* get_method_at(class_file* class_file, u4 index){
    return class_file->methods + index;
}


