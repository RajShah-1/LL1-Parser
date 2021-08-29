#!/bin/bash
mkdir -p build
mkdir -p build/Module_2
g++ -c -o ./build/Module_2/LL1Parser.out ./src/Module_2/LL1Parser.cpp
g++ -c -o ./build/Module_2/mainTmp.out ./src/Module_2/main.cpp
g++ -o ./build/Module_2/main.out ./build/Module_2/LL1Parser.out ./build/Module_2/mainTmp.out
