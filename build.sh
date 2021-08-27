#!/bin/bash
mkdir -p build
g++ -c -o ./build/LL1.out ./src/LL1.cpp
g++ -c -o ./build/mainTmp.out ./src/main.cpp
g++ -o ./build/main.out ./build/LL1.out ./build/mainTmp.out 
