# About

This software provides the direction and speed of a moving object using two images as input.
The OpenCV framework is needed to use this software.

http://ra.fael.nl

# Compiling (Linux / MacOS)

g++ -c flow.cpp -o flow.o `pkg-config --cflags --libs opencv`
g++ flow.o main.cpp -o flow.bin `pkg-config --cflags --libs opencv`