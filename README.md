# Naive JVM implementation in C

## Features
* Class file parser
* Function calls
* Zero dependency(even libc)
* Implemented java method `System.out.print.print(int)` for debugging

## Unimplemented features
* Jar file
* Native methods
* Class loader

## Build requirements
* GCC
* JDK 1.8+ to build the test class file

## How to build

```console

$ make class_file_test # test the class file parser
$ make vm_test # test the vm

```


