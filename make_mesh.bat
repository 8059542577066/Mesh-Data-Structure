@echo off
rem gcc 9.2.0 (tdm64) win10
g++ mesh.cpp -O3 -std=c++11 -Wall -pedantic -DBUILD_LIB -shared -o mesh.dll
pause