#!/bin/bash

# Compile first
echo "Compiling..."
g++ -Wall -ICode/include Code/src/*.cpp -o main

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

echo "Compilation successful!"
echo ""

# Check arguments
if [ "$#" -lt 1 ]; then
    echo "Usage: ./run_batch.sh input.csv [risk.csv]"
    echo "Example: ./run_batch.sh dataset/data.csv dataset/risk.csv"
    exit 1
fi

if [ "$1" == "-b" ]; then
    shift
fi

INPUT_FILE=$1
RISK_FILE=${2:-""}   # optional, empty if not provided

# Run in batch mode
if [ -z "$RISK_FILE" ]; then
    echo "Running batch mode (no risk file)..."
    ./main -b "$INPUT_FILE"
else
    echo "Running batch mode with risk analysis..."
    ./main -b "$INPUT_FILE" "$RISK_FILE"
fi

if [ $? -ne 0 ]; then
    echo "Program failed!"
    exit 1
fi

echo ""
echo "Done! Check output file for results."