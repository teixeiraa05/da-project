#!/bin/bash
echo "Compiling..."
g++ -Wall -I Code/include Code/src/*.cpp -o main

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running..."
    ./main
else
    echo "Compilation failed."
fi
