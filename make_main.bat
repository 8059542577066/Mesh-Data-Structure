@echo off
rem gcc 9.2.0 (tdm64) win10
g++ main.cpp -O3 -std=c++11 -Wall -pedantic -L./ -lmesh -lfile -o main.exe
pause