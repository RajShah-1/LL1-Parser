#!/bin/bash
mkdir -p build
mkdir -p build/Module_1
g++ -c -o ./build/Module_1/LL1.out ./src/Module_1/LL1.cpp
g++ -c -o ./build/Module_1/mainTmp.out ./src/Module_1/main.cpp
g++ -o ./build/Module_1/main.out ./build/Module_1/LL1.out ./build/Module_1/mainTmp.out
