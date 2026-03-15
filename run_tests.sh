#!/bin/bash
echo "Compiling..."
g++ -Wall -I./Code/include ./Code/src/Parser.cpp ./tests/test_parser.cpp -o test_parser

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running..."
    ./test_parser
else
    echo "Compilation failed."
fi